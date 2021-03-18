/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* SPDX-FileCopyrightText: 2017-2021 Bartosz Golaszewski <bartekgola@gmail.com> */

#ifndef __LIBGPIOD_GPIOD_CXX_HPP__
#define __LIBGPIOD_GPIOD_CXX_HPP__

#include <bitset>
#include <chrono>
#include <gpiod.h>
#include <memory>
#include <string>
#include <vector>

namespace gpiod {

class line;
class line_bulk;
class line_iter;
class chip_iter;
struct line_event;

/**
 * @file gpiod.hpp
 */

/**
 * @defgroup gpiod_cxx C++ bindings
 * @{
 */

/**
 * @brief Check if the file pointed to by path is a GPIO chip character device.
 * @param path Path to check.
 * @return True if the file exists and is a GPIO chip character device or a
 *         symbolic link to it.
 */
bool is_gpiochip_device(const ::std::string& path);

/**
 * @brief Represents a GPIO chip.
 *
 * Internally this class holds a smart pointer to an open GPIO chip descriptor.
 * Multiple objects of this class can reference the same chip. The chip is
 * closed and all resources freed when the last reference is dropped.
 */
class chip
{
public:

	/**
	 * @brief Default constructor. Creates an empty GPIO chip object.
	 */
	chip(void) = default;

	/**
	 * @brief Constructor. Opens the chip using chip::open.
	 * @param path Path to the GPIO chip device.
	 */
	chip(const ::std::string& path);

	/**
	 * @brief Copy constructor. References the object held by other.
	 * @param other Other chip object.
	 */
	chip(const chip& other) = default;

	/**
	 * @brief Move constructor. References the object held by other.
	 * @param other Other chip object.
	 */
	chip(chip&& other) = default;

	/**
	 * @brief Assignment operator. References the object held by other.
	 * @param other Other chip object.
	 * @return Reference to this object.
	 */
	chip& operator=(const chip& other) = default;

	/**
	 * @brief Move assignment operator. References the object held by other.
	 * @param other Other chip object.
	 * @return Reference to this object.
	 */
	chip& operator=(chip&& other) = default;

	/**
	 * @brief Destructor. Unreferences the internal chip object.
	 */
	~chip(void) = default;

	/**
	 * @brief Open a GPIO chip.
	 * @param path Path to the GPIO chip device.
	 *
	 * If the object already holds a reference to an open chip, it will be
	 * closed and the reference reset.
	 */
	void open(const ::std::string &path);

	/**
	 * @brief Reset the internal smart pointer owned by this object.
	 */
	void reset(void) noexcept;

	/**
	 * @brief Return the name of the chip held by this object.
	 * @return Name of the GPIO chip.
	 */
	::std::string name(void) const;

	/**
	 * @brief Return the label of the chip held by this object.
	 * @return Label of the GPIO chip.
	 */
	::std::string label(void) const;

	/**
	 * @brief Return the number of lines exposed by this chip.
	 * @return Number of lines.
	 */
	unsigned int num_lines(void) const;

	/**
	 * @brief Get the line exposed by this chip at given offset.
	 * @param offset Offset of the line.
	 * @return Line object.
	 */
	line get_line(unsigned int offset) const;

	/**
	 * @brief Map a GPIO line's name to its offset within the chip.
	 * @param name Name of the GPIO line to map.
	 * @return Offset of the line within the chip or -1 if a line with
	 *         given name is not exposed by the chip.
	 */
	int find_line(const ::std::string& name) const;

	/**
	 * @brief Get a set of lines exposed by this chip at given offsets.
	 * @param offsets Vector of line offsets.
	 * @return Set of lines held by a line_bulk object.
	 */
	line_bulk get_lines(const ::std::vector<unsigned int>& offsets) const;

	/**
	 * @brief Get all lines exposed by this chip.
	 * @return All lines exposed by this chip held by a line_bulk object.
	 */
	line_bulk get_all_lines(void) const;

	/**
	 * @brief Equality operator.
	 * @param rhs Right-hand side of the equation.
	 * @return True if rhs references the same chip. False otherwise.
	 */
	bool operator==(const chip& rhs) const noexcept;

	/**
	 * @brief Inequality operator.
	 * @param rhs Right-hand side of the equation.
	 * @return False if rhs references the same chip. True otherwise.
	 */
	bool operator!=(const chip& rhs) const noexcept;

	/**
	 * @brief Check if this object holds a reference to a GPIO chip.
	 * @return True if this object references a GPIO chip, false otherwise.
	 */
	explicit operator bool(void) const noexcept;

	/**
	 * @brief Check if this object doesn't hold a reference to a GPIO chip.
	 * @return False if this object references a GPIO chip, true otherwise.
	 */
	bool operator!(void) const noexcept;

private:

	chip(::gpiod_chip* chip);
	chip(const ::std::weak_ptr<::gpiod_chip>& chip_ptr);

	void throw_if_noref(void) const;

	::std::shared_ptr<::gpiod_chip> _m_chip;

	friend line;
	friend chip_iter;
	friend line_iter;
};

/**
 * @brief Stores the configuration for line requests.
 */
struct line_request
{
	/**
	 * @brief Request types.
	 */
	enum : int {
		DIRECTION_AS_IS = 1,
		/**< Request for values, don't change the direction. */
		DIRECTION_INPUT,
		/**< Request for reading line values. */
		DIRECTION_OUTPUT,
		/**< Request for driving the GPIO lines. */
		EVENT_FALLING_EDGE,
		/**< Listen for falling edge events. */
		EVENT_RISING_EDGE,
		/**< Listen for rising edge events. */
		EVENT_BOTH_EDGES,
		/**< Listen for all types of events. */
	};

	static const ::std::bitset<32> FLAG_ACTIVE_LOW;
	/**< Set the active state to 'low' (high is the default). */
	static const ::std::bitset<32> FLAG_OPEN_SOURCE;
	/**< The line is an open-source port. */
	static const ::std::bitset<32> FLAG_OPEN_DRAIN;
	/**< The line is an open-drain port. */
	static const ::std::bitset<32> FLAG_BIAS_DISABLED;
	/**< The line has neither pull-up nor pull-down resistor enabled. */
	static const ::std::bitset<32> FLAG_BIAS_PULL_DOWN;
	/**< The line has a configurable pull-down resistor enabled. */
	static const ::std::bitset<32> FLAG_BIAS_PULL_UP;
	/**< The line has a configurable pull-up resistor enabled. */

	::std::string consumer;
	/**< Consumer name to pass to the request. */
	int request_type;
	/**< Type of the request. */
	::std::bitset<32> flags;
	/**< Additional request flags. */
};

/**
 * @brief Represents a single GPIO line.
 *
 * Internally this class holds a raw pointer to a GPIO line descriptor and a
 * reference to the parent chip. All line resources are freed when the last
 * reference to the parent chip is dropped.
 */
class line
{
public:

	/**
	 * @brief Default constructor. Creates an empty line object.
	 */
	line(void);

	/**
	 * @brief Copy constructor.
	 * @param other Other line object.
	 */
	line(const line& other) = default;

	/**
	 * @brief Move constructor.
	 * @param other Other line object.
	 */
	line(line&& other) = default;

	/**
	 * @brief Assignment operator.
	 * @param other Other line object.
	 * @return Reference to this object.
	 */
	line& operator=(const line& other) = default;

	/**
	 * @brief Move assignment operator.
	 * @param other Other line object.
	 * @return Reference to this object.
	 */
	line& operator=(line&& other) = default;

	/**
	 * @brief Destructor.
	 */
	~line(void) = default;

	/**
	 * @brief Get the offset of this line.
	 * @return Offet of this line.
	 */
	unsigned int offset(void) const;

	/**
	 * @brief Get the name of this line (if any).
	 * @return Name of this line or an empty string if it is unnamed.
	 */
	::std::string name(void) const;

	/**
	 * @brief Get the consumer of this line (if any).
	 * @return Name of the consumer of this line or an empty string if it
	 *         is unused.
	 */
	::std::string consumer(void) const;

	/**
	 * @brief Get current direction of this line.
	 * @return Current direction setting.
	 */
	int direction(void) const;

	/**
	 * @brief Check if this line's signal is inverted.
	 * @return True if this line is "active-low", false otherwise.
	 */
	bool is_active_low(void) const;

	/**
	 * @brief Get current bias of this line.
	 * @return Current bias setting.
	 */
	int bias(void) const;

	/**
	 * @brief Check if this line is used by the kernel or other user space
	 *        process.
	 * @return True if this line is in use, false otherwise.
	 */
	bool is_used(void) const;

	/**
	 * @brief Get current drive setting of this line.
	 * @return Current drive setting.
	 */
	int drive(void) const;

	/**
	 * @brief Request this line.
	 * @param config Request config (see gpiod::line_request).
	 * @param default_val Default value - only matters for OUTPUT direction.
	 */
	void request(const line_request& config, int default_val = 0) const;

	/**
	 * @brief Release the line if it was previously requested.
	 */
	void release(void) const;

	/**
	 * @brief Read the line value.
	 * @return Current value (0 or 1).
	 */
	int get_value(void) const;

	/**
	 * @brief Set the value of this line.
	 * @param val New value (0 or 1).
	 */
	void set_value(int val) const;

	/**
	 * @brief Set configuration of this line.
	 * @param direction New direction.
	 * @param flags Replacement flags.
	 * @param value New value (0 or 1) - only matters for OUTPUT direction.
	 */
	void set_config(int direction, ::std::bitset<32> flags, int value = 0) const;

	/**
	 * @brief Set configuration flags of this line.
	 * @param flags Replacement flags.
	 */
	void set_flags(::std::bitset<32> flags) const;

	/**
	 * @brief Change the direction this line to input.
	 */
	void set_direction_input() const;

	/**
	 * @brief Change the direction this lines to output.
	 * @param value New value (0 or 1).
	 */
	void set_direction_output(int value = 0) const;

	/**
	 * @brief Wait for an event on this line.
	 * @param timeout Time to wait before returning if no event occurred.
	 * @return True if an event occurred and can be read, false if the wait
	 *         timed out.
	 */
	bool event_wait(const ::std::chrono::nanoseconds& timeout) const;

	/**
	 * @brief Read a line event.
	 * @return Line event object.
	 */
	line_event event_read(void) const;

	/**
	 * @brief Read multiple line events.
	 * @return Vector of line event objects.
	 */
	::std::vector<line_event> event_read_multiple(void) const;

	/**
	 * @brief Get the event file descriptor associated with this line.
	 * @return File descriptor number.
	 */
	int event_get_fd(void) const;

	/**
	 * @brief Get the parent chip.
	 * @return Parent chip of this line.
	 */
	const chip get_chip(void) const;

	/**
	 * @brief Reset the state of this object.
	 *
	 * This is useful when the user needs to e.g. keep the line_event object
	 * but wants to drop the reference to the GPIO chip indirectly held by
	 * the line being the source of the event.
	 */
	void reset(void);

	/**
	 * @brief Check if two line objects reference the same GPIO line.
	 * @param rhs Right-hand side of the equation.
	 * @return True if both objects reference the same line, fale otherwise.
	 */
	bool operator==(const line& rhs) const noexcept;

	/**
	 * @brief Check if two line objects reference different GPIO lines.
	 * @param rhs Right-hand side of the equation.
	 * @return False if both objects reference the same line, true otherwise.
	 */
	bool operator!=(const line& rhs) const noexcept;

	/**
	 * @brief Check if this object holds a reference to any GPIO line.
	 * @return True if this object references a GPIO line, false otherwise.
	 */
	explicit operator bool(void) const noexcept;

	/**
	 * @brief Check if this object doesn't reference any GPIO line.
	 * @return True if this object doesn't reference any GPIO line, true
	 *         otherwise.
	 */
	bool operator!(void) const noexcept;

	/**
	 * @brief Possible direction settings.
	 */
	enum : int {
		DIRECTION_INPUT = 1,
		/**< Line's direction setting is input. */
		DIRECTION_OUTPUT,
		/**< Line's direction setting is output. */
	};

	/**
	 * @brief Possible drive settings.
	 */
	enum : int {
		DRIVE_PUSH_PULL = 1,
		/**< Drive setting is unknown. */
		DRIVE_OPEN_DRAIN,
		/**< Line output is open-drain. */
		DRIVE_OPEN_SOURCE,
		/**< Line output is open-source. */
	};

	/**
	 * @brief Possible bias settings.
	 */
	enum : int {
		BIAS_UNKNOWN = 1,
		/**< Line's bias state is unknown. */
		BIAS_DISABLED,
		/**< Line's internal bias is disabled. */
		BIAS_PULL_UP,
		/**< Line's internal pull-up bias is enabled. */
		BIAS_PULL_DOWN,
		/**< Line's internal pull-down bias is enabled. */
	};

private:

	line(::gpiod_line* line, const chip& owner);

	void throw_if_null(void) const;
	line_event make_line_event(const ::gpiod_line_event& event) const noexcept;

	::gpiod_line* _m_line;
	::std::weak_ptr<::gpiod_chip> _m_owner;

	class chip_guard
	{
	public:
		chip_guard(const line& line);
		~chip_guard(void) = default;

		chip_guard(const chip_guard& other) = delete;
		chip_guard(chip_guard&& other) = delete;
		chip_guard& operator=(const chip_guard&& other) = delete;
		chip_guard& operator=(chip_guard&& other) = delete;

	private:
		::std::shared_ptr<::gpiod_chip> _m_chip;
	};

	friend chip;
	friend line_bulk;
	friend line_iter;
};

/**
 * @brief Describes a single GPIO line event.
 */
struct line_event
{
	/**
	 * @brief Possible event types.
	 */
	enum : int {
		RISING_EDGE = 1,
		/**< Rising edge event. */
		FALLING_EDGE,
		/**< Falling edge event. */
	};

	::std::chrono::nanoseconds timestamp;
	/**< Best estimate of time of event occurrence in nanoseconds. */
	int event_type;
	/**< Type of the event that occurred. */
	line source;
	/**< Line object referencing the GPIO line on which the event occurred. */
};

/**
 * @brief Represents a set of GPIO lines.
 *
 * Internally an object of this class stores an array of line objects
 * owned by a single chip.
 */
class line_bulk
{
public:

	/**
	 * @brief Default constructor. Creates an empty line_bulk object.
	 */
	line_bulk(void) = default;

	/**
	 * @brief Construct a line_bulk from a vector of lines.
	 * @param lines Vector of gpiod::line objects.
	 * @note All lines must be owned by the same GPIO chip.
	 */
	line_bulk(const ::std::vector<line>& lines);

	/**
	 * @brief Copy constructor.
	 * @param other Other line_bulk object.
	 */
	line_bulk(const line_bulk& other) = default;

	/**
	 * @brief Move constructor.
	 * @param other Other line_bulk object.
	 */
	line_bulk(line_bulk&& other) = default;

	/**
	 * @brief Assignment operator.
	 * @param other Other line_bulk object.
	 * @return Reference to this object.
	 */
	line_bulk& operator=(const line_bulk& other) = default;

	/**
	 * @brief Move assignment operator.
	 * @param other Other line_bulk object.
	 * @return Reference to this object.
	 */
	line_bulk& operator=(line_bulk&& other) = default;

	/**
	 * @brief Destructor.
	 */
	~line_bulk(void) = default;

	/**
	 * @brief Add a line to this line_bulk object.
	 * @param new_line Line to add.
	 * @note The new line must be owned by the same chip as all the other
	 *       lines already held by this line_bulk object.
	 */
	void append(const line& new_line);

	/**
	 * @brief Get the line at given offset.
	 * @param index Index of the line to get.
	 * @return Reference to the line object.
	 * @note This method will throw if index is equal or greater than the
	 *       number of lines currently held by this bulk.
	 */
	line& get(unsigned int index);

	/**
	 * @brief Get the line at given offset without bounds checking.
	 * @param index Offset of the line to get.
	 * @return Reference to the line object.
	 * @note No bounds checking is performed.
	 */
	line& operator[](unsigned int index);

	/**
	 * @brief Get the number of lines currently held by this object.
	 * @return Number of elements in this line_bulk.
	 */
	unsigned int size(void) const noexcept;

	/**
	 * @brief Check if this line_bulk doesn't hold any lines.
	 * @return True if this object is empty, false otherwise.
	 */
	bool empty(void) const noexcept;

	/**
	 * @brief Remove all lines from this object.
	 */
	void clear(void);

	/**
	 * @brief Request all lines held by this object.
	 * @param config Request config (see gpiod::line_request).
	 * @param default_vals Vector of default values. Only relevant for
	 *                     output direction requests.
	 */
	void request(const line_request& config,
		     const ::std::vector<int> default_vals = ::std::vector<int>()) const;

	/**
	 * @brief Release all lines held by this object.
	 */
	void release(void) const;

	/**
	 * @brief Read values from all lines held by this object.
	 * @return Vector containing line values the order of which corresponds
	 *         with the order of lines in the internal array.
	 */
	::std::vector<int> get_values(void) const;

	/**
	 * @brief Set values of all lines held by this object.
	 * @param values Vector of values to set. Must be the same size as the
	 *               number of lines held by this line_bulk.
	 */
	void set_values(const ::std::vector<int>& values) const;

	/**
	 * @brief Set configuration of all lines held by this object.
	 * @param direction New direction.
	 * @param flags Replacement flags.
	 * @param values Vector of values to set. Must be the same size as the
	 *               number of lines held by this line_bulk.
	 *               Only relevant for output direction requests.
	 */
	void set_config(int direction, ::std::bitset<32> flags,
			const ::std::vector<int> values = ::std::vector<int>()) const;

	/**
	 * @brief Set configuration flags of all lines held by this object.
	 * @param flags Replacement flags.
	 */
	void set_flags(::std::bitset<32> flags) const;

	/**
	 * @brief Change the direction all lines held by this object to input.
	 */
	void set_direction_input() const;

	/**
	 * @brief Change the direction all lines held by this object to output.
	 * @param values Vector of values to set. Must be the same size as the
	 *               number of lines held by this line_bulk.
	 */
	void set_direction_output(const ::std::vector<int>& values) const;

	/**
	 * @brief Poll the set of lines for line events.
	 * @param timeout Number of nanoseconds to wait before returning an
	 *                empty line_bulk.
	 * @return Returns a line_bulk object containing lines on which events
	 *         occurred.
	 */
	line_bulk event_wait(const ::std::chrono::nanoseconds& timeout) const;

	/**
	 * @brief Check if this object holds any lines.
	 * @return True if this line_bulk holds at least one line, false otherwise.
	 */
	explicit operator bool(void) const noexcept;

	/**
	 * @brief Check if this object doesn't hold any lines.
	 * @return True if this line_bulk is empty, false otherwise.
	 */
	bool operator!(void) const noexcept;

	/**
	 * @brief Max number of lines that this object can hold.
	 */
	static const unsigned int MAX_LINES;

	/**
	 * @brief Iterator for iterating over lines held by line_bulk.
	 */
	class iterator
	{
	public:

		/**
		 * @brief Default constructor. Builds an empty iterator object.
		 */
		iterator(void) = default;

		/**
		 * @brief Copy constructor.
		 * @param other Other line_bulk iterator.
		 */
		iterator(const iterator& other) = default;

		/**
		 * @brief Move constructor.
		 * @param other Other line_bulk iterator.
		 */
		iterator(iterator&& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param other Other line_bulk iterator.
		 * @return Reference to this iterator.
		 */
		iterator& operator=(const iterator& other) = default;

		/**
		 * @brief Move assignment operator.
		 * @param other Other line_bulk iterator.
		 * @return Reference to this iterator.
		 */
		iterator& operator=(iterator&& other) = default;

		/**
		 * @brief Destructor.
		 */
		~iterator(void) = default;

		/**
		 * @brief Advance the iterator by one element.
		 * @return Reference to this iterator.
		 */
		iterator& operator++(void);

		/**
		 * @brief Dereference current element.
		 * @return Current GPIO line by reference.
		 */
		const line& operator*(void) const;

		/**
		 * @brief Member access operator.
		 * @return Current GPIO line by pointer.
		 */
		const line* operator->(void) const;

		/**
		 * @brief Check if this operator points to the same element.
		 * @param rhs Right-hand side of the equation.
		 * @return True if this iterator points to the same GPIO line,
		 *         false otherwise.
		 */
		bool operator==(const iterator& rhs) const noexcept;

		/**
		 * @brief Check if this operator doesn't point to the same element.
		 * @param rhs Right-hand side of the equation.
		 * @return True if this iterator doesn't point to the same GPIO
		 *         line, false otherwise.
		 */
		bool operator!=(const iterator& rhs) const noexcept;

	private:

		iterator(const ::std::vector<line>::iterator& it);

		::std::vector<line>::iterator _m_iter;

		friend line_bulk;
	};

	/**
	 * @brief Returns an iterator to the first line.
	 * @return A line_bulk iterator.
	 */
	iterator begin(void) noexcept;

	/**
	 * @brief Returns an iterator to the element following the last line.
	 * @return A line_bulk iterator.
	 */
	iterator end(void) noexcept;

private:

	struct line_bulk_deleter
	{
		void operator()(::gpiod_line_bulk *bulk);
	};

	void throw_if_empty(void) const;

	using line_bulk_ptr = ::std::unique_ptr<::gpiod_line_bulk, line_bulk_deleter>;

	line_bulk_ptr make_line_bulk_ptr(void) const;
	line_bulk_ptr to_line_bulk(void) const;

	::std::vector<line> _m_bulk;
};

/**
 * @brief Support for range-based loops for line iterators.
 * @param iter A line iterator.
 * @return Iterator unchanged.
 */
line_iter begin(line_iter iter) noexcept;

/**
 * @brief Support for range-based loops for line iterators.
 * @param iter A line iterator.
 * @return New end iterator.
 */
line_iter end(const line_iter& iter) noexcept;

/**
 * @brief Allows to iterate over all lines owned by a GPIO chip.
 */
class line_iter
{
public:

	/**
	 * @brief Default constructor. Creates the end iterator.
	 */
	line_iter(void) = default;

	/**
	 * @brief Constructor. Creates the begin iterator.
	 * @param owner Chip owning the GPIO lines over which we want to iterate.
	 */
	line_iter(const chip& owner);

	/**
	 * @brief Copy constructor.
	 * @param other Other line iterator.
	 */
	line_iter(const line_iter& other) = default;

	/**
	 * @brief Move constructor.
	 * @param other Other line iterator.
	 */
	line_iter(line_iter&& other) = default;

	/**
	 * @brief Assignment operator.
	 * @param other Other line iterator.
	 * @return Reference to this line_iter.
	 */
	line_iter& operator=(const line_iter& other) = default;

	/**
	 * @brief Move assignment operator.
	 * @param other Other line iterator.
	 * @return Reference to this line_iter.
	 */
	line_iter& operator=(line_iter&& other) = default;

	/**
	 * @brief Destructor.
	 */
	~line_iter(void) = default;

	/**
	 * @brief Advance the iterator by one element.
	 * @return Reference to this iterator.
	 */
	line_iter& operator++(void);

	/**
	 * @brief Dereference current element.
	 * @return Current GPIO line by reference.
	 */
	const line& operator*(void) const;

	/**
	 * @brief Member access operator.
	 * @return Current GPIO line by pointer.
	 */
	const line* operator->(void) const;

	/**
	 * @brief Check if this operator points to the same element.
	 * @param rhs Right-hand side of the equation.
	 * @return True if this iterator points to the same line_iter,
	 *         false otherwise.
	 */
	bool operator==(const line_iter& rhs) const noexcept;

	/**
	 * @brief Check if this operator doesn't point to the same element.
	 * @param rhs Right-hand side of the equation.
	 * @return True if this iterator doesn't point to the same line_iter,
	 *         false otherwise.
	 */
	bool operator!=(const line_iter& rhs) const noexcept;

private:

	line _m_current;
};

/**
 * @}
 */

} /* namespace gpiod */

#endif /* __LIBGPIOD_GPIOD_CXX_HPP__ */
