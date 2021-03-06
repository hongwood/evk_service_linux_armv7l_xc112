// Copyright (c) Acconeer AB, 2016-2018
// All rights reserved

#ifndef ACC_OS_H_
#define ACC_OS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h> // For strrchr
#include <time.h>

#if defined(TARGET_OS_linux) || defined(__MINGW32__)
#include <unistd.h>
#endif

#include "acc_os_debug.h"
#include "acc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t	acc_os_thread_id_t;
typedef uint32_t	acc_os_net_address_t;
typedef uint16_t	acc_os_net_port_t;

struct acc_os_mutex;
typedef struct acc_os_mutex *acc_os_mutex_t;

struct acc_os_semaphore;
typedef struct acc_os_semaphore *acc_os_semaphore_t;

struct acc_os_socket;
typedef struct acc_os_socket *acc_os_socket_t;

struct acc_os_thread_handle;
typedef struct acc_os_thread_handle *acc_os_thread_handle_t;

// These functions are to be used by drivers only, do not use them directly
extern void                   (*acc_device_os_init_func)(void);
extern void                   (*acc_device_os_stack_setup_func)(size_t stack_size);
extern size_t                 (*acc_device_os_stack_get_usage_func)(size_t stack_size);
extern void                   (*acc_device_os_sleep_us_func)(uint32_t time_usec);
extern void*                  (*acc_device_os_get_mem_alloc_func)(size_t);
extern void                   (*acc_device_os_get_mem_free_func)(void *);
extern acc_os_thread_id_t     (*acc_device_os_get_thread_id_func)(void);
extern void                   (*acc_device_os_localtime_func)(struct tm *time_tm, uint32_t *time_usec);
extern acc_os_mutex_t         (*acc_device_os_mutex_create_func)();
extern void                   (*acc_device_os_mutex_lock_func)(acc_os_mutex_t mutex);
extern void                   (*acc_device_os_mutex_unlock_func)(acc_os_mutex_t mutex);
extern void                   (*acc_device_os_mutex_destroy_func)(acc_os_mutex_t mutex);
extern acc_os_thread_handle_t (*acc_device_os_thread_create_func)(void (*func)(void *param), void *param);
extern void                   (*acc_device_os_thread_delete_func)(void);
extern bool                   (*acc_device_os_thread_cleanup_func)(acc_os_thread_handle_t handle);
extern void*                  (*acc_device_os_dynamic_open_func)(char *filename);
extern void                   (*acc_device_os_dynamic_close_func)(void *handle);
extern void*                  (*acc_device_os_dynamic_symbol_func)(void *handle, char *name);
extern char*                  (*acc_device_os_dynamic_error_func)(void *handle);
extern acc_os_net_address_t   (*acc_device_os_net_string_to_address_func)(const char *str);
extern void                   (*acc_device_os_net_address_to_string_func)(acc_os_net_address_t address, char *buffer, size_t max_size);
extern acc_os_socket_t        (*acc_device_os_net_connect_func)(acc_os_net_address_t address, acc_os_net_port_t port);
extern void                   (*acc_device_os_net_disconnect_func)(acc_os_socket_t sock);
extern int                    (*acc_device_os_net_send_func)(acc_os_socket_t sock, void *buffer, size_t size);
extern int                    (*acc_device_os_net_receive_func)(acc_os_socket_t sock, void *buffer, size_t max_size, uint_fast32_t timeout_us);
extern void                   (*acc_device_os_set_socket_invalid_func)(acc_os_socket_t sock);
extern bool                   (*acc_device_os_is_socket_valid_func)(acc_os_socket_t sock);
extern acc_os_semaphore_t     (*acc_device_os_semaphore_create_func)(void);
extern int_fast8_t            (*acc_device_os_semaphore_wait_func)(acc_os_semaphore_t sem, uint_fast16_t timeout_ms);
extern void                   (*acc_device_os_semaphore_signal_func)(acc_os_semaphore_t sem);
extern void                   (*acc_device_os_semaphore_signal_from_interrupt_func)(acc_os_semaphore_t sem);
extern void                   (*acc_device_os_semaphore_destroy_func)(acc_os_semaphore_t sem);
extern uint16_t 		(*acc_device_os_ntohs_func)(uint16_t value);
extern uint16_t 		(*acc_device_os_htons_func)(uint16_t value);
extern uint32_t 		(*acc_device_os_ntohl_func)(uint32_t value);
extern uint32_t 		(*acc_device_os_htonl_func)(uint32_t value);


/**
 * @brief Perform any os specific initialization
 */
extern void acc_os_init(void);

/**
 * @brief Prepare stack for measuring stack usage - to be called as early as possible
 *
 * @param stack_size Amount of stack in bytes that is allocated
 */
extern void acc_os_stack_setup(size_t stack_size);

/**
 * @brief Measure amount of used stack in bytes
 *
 * @param stack_size Amount of stack in bytes that is allocated
 * @return Number of bytes of used stack space
 */
extern size_t acc_os_stack_get_usage(size_t stack_size);

/**
 * @brief Sleep for a specified number of microseconds
 *
 * @param time_usec Time in microseconds to sleep
 */
extern void acc_os_sleep_us(uint32_t time_usec);


/**
 * @brief Allocate dynamic memory
 *
 * Use platform specific mechanism to allocate dynamic memory. The memory is guaranteed
 * to be naturally aligned. Requesting zero bytes will return NULL.
 *
 * On error, NULL is returned.
 *
 * @param size The number of bytes to allocate
 * @param file The file which makes the allocation
 * @param line The line where this allocation takes place
 * @return Pointer to the allocated memory, or NULL if allocation failed
 */
#define REL_FILE_PATH (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define acc_os_mem_alloc(X) acc_os_mem_alloc_debug(X, REL_FILE_PATH, __LINE__)
extern void *acc_os_mem_alloc_debug(size_t size, const char *file, uint16_t line);


/**
 * @brief Allocate dynamic memory and initialize it with zeros.
 *
 * Use platform specific mechanism to allocate dynamic memory. The memory is guaranteed
 * to be naturally aligned. If either num or size is zero, NULL is returned.
 *
 * On error, NULL is returned.
 *
 * @param num The number of elements to allocate.
 * @param size The size of each element.
 * @param file The file which makes the allocation.
 * @param line The line where this allocation takes place
 * @return Pointer to the allocated memory, or NULL if allocation failed
 */
extern void *acc_os_mem_calloc_debug(size_t num, size_t size, const char *file, uint16_t line);
#define acc_os_mem_calloc(X, Y) acc_os_mem_calloc_debug(X, Y, REL_FILE_PATH, __LINE__)


/**
 * @brief Free dynamic memory allocated with acc_os_mem_alloc
 *
 * Use platform specific mechanism to free dynamic memory. Passing NULL is allowed
 * but will do nothing.
 *
 * Freeing memory not allocated with acc_os_mem_alloc, or freeing memory already
 * freed, will result in undefined behaviour.
 *
 * @param ptr Pointer to the dynamic memory to free
 */
extern void acc_os_mem_free(void *ptr);

/**
 * @brief Return the unique thread ID for the current thread
 */
extern acc_os_thread_id_t acc_os_get_thread_id(void);

/**
 * @brief Calculate current time and return in a struct tm, and optionally with microseconds
 *
 * @param time_tm	The local time, as struct tm, is returned here
 * @param time_usec	If not NULL, the microsecond part of the time is returned here
 */
extern void acc_os_localtime(struct tm *time_tm, uint32_t *time_usec);

/**
 * @brief Initialize a mutex
 *
 * If a pointer to a mutex is given, that mutex is initialized. If NULL is given, a new
 * mutex is dynamically allocated and initialized.
 *
 * If the pointer to an already initialized mutex is given, nothing is done and the mutex is
 * left in its current state. Care must therefore be taken that the mutex variable is zeroed
 * before being passed to acc_os_mutex_init() so it can be detected to be initialized or not.
 *
 * @return Newly initialized mutex
 */
extern acc_os_mutex_t acc_os_mutex_create(void);

/**
 * @brief Mutex lock
 *
 * @param mutex Mutex to be locked
 */
extern void acc_os_mutex_lock(acc_os_mutex_t mutex);

/**
 * @brief Mutex unlock
 *
 * @param mutex Mutex to be unlocked
 */
extern void acc_os_mutex_unlock(acc_os_mutex_t mutex);


/**
 * @brief Destroys the mutex
 *
 * @param mutex Mutex to be destroyed
 */
extern void acc_os_mutex_destroy(acc_os_mutex_t mutex);


/**
 * @brief Create new thread
 *
 * @param func	Function implementing the thread code
 * @param param	Parameter to be passed to the thread function
 * @return A handle to a newly created thread
 */
extern acc_os_thread_handle_t acc_os_thread_create(void (*func)(void *param), void *param);

/**
 * @brief Delete current thread
 *
 * There is no return status. If the function returns, it failed.
 */
extern void acc_os_thread_delete(void);

/**
 * @brief Cleanup after thread termination
 *
 * For operating systems that require it, perform any post-thread cleanup operation.
 *
 * @param thread Handle of thread
 * @return True if the cleanup succeeded
 */
extern bool acc_os_thread_cleanup(acc_os_thread_handle_t thread);

/**
 * @brief Open a dynamic library, returning a handle to the library
 *
 * @param  filename Name of dynamic library file to be opened
 * @return A handle to be used for successive calls
 */
extern void *acc_os_dynamic_open(char *filename);

/**
 * @brief Close a previously opened dynamic library
 *
 * @param  handle The handle returned from acc_os_dynamic_open()
 */
extern void acc_os_dynamic_close(void *handle);

/**
 * @brief Lookup a symbol in a dynamic library, returns NULL on failure
 *
 * @param  handle The handle returned from acc_os_dynamic_open()
 * @param  name   The name of the symbol to find in the library
 * @return The value of the symbol, if found
 */
extern void *acc_os_dynamic_symbol(void *handle, char *name);

/**
 * @brief Return a pointer to a string describing the last error, or NULL if no error
 *
 * @param  handle The handle returned by acc_os_dynamic_open()
 * @return A pointer to the error string
 */
extern char *acc_os_dynamic_error(void *handle);

extern acc_os_net_address_t acc_os_net_string_to_address(const char *str);
extern void acc_os_net_address_to_string(acc_os_net_address_t address, char *buffer, size_t max_size);
extern acc_os_socket_t acc_os_net_connect(acc_os_net_address_t address, acc_os_net_port_t port);
extern void acc_os_set_socket_invalid(acc_os_socket_t sock);
extern bool acc_os_is_socket_valid(acc_os_socket_t sock);
extern void acc_os_net_disconnect(acc_os_socket_t sock);
extern int acc_os_net_send(acc_os_socket_t sock, void *buffer, size_t size);
extern int acc_os_net_receive(acc_os_socket_t sock, void *buffer, size_t max_size, uint_fast32_t timeout_us);

/**
 * @brief Creates a semaphore and returns a pointer to the newly created semaphore
 *
 * @return A pointer to the semaphore on success otherwise NULL
 */
extern acc_os_semaphore_t acc_os_semaphore_create(void);

/**
 * @brief Waits for the semaphore to be available. The task calling this function will be
 * blocked until the semaphore is signaled from another task.
 *
 * @param[in]  sem A pointer to the semaphore to use
 * @param[in]  timeout_ms The amount of time to wait before a timeout occurs
 * @return Returns 0 on success otherwise -1
 */
extern int_fast8_t acc_os_semaphore_wait(acc_os_semaphore_t sem, uint_fast16_t timeout_ms);

/**
 * @brief Signal the semaphore. Not ISR safe. If needed from an ISR
 * use acc_os_semaphore_signal_from_interrupt instead. Releases the semaphore resulting
 * in a release of the task that is blocked waiting for the semaphore.
 *
 * @param[in]  sem A pointer to the semaphore to signal
 */
extern void acc_os_semaphore_signal(acc_os_semaphore_t sem);

/**
 * @brief Signal the semaphore. This routine is safe to call from an
 * ISR routine
 *
 * @param[in]  sem A pointer to the semaphore to signal
 */
extern void acc_os_semaphore_signal_from_interrupt(acc_os_semaphore_t sem);

/**
 * @brief Deallocates the semaphore
 *
 * @param[in]  sem A pointer to the semaphore to deallocate
 */
extern void acc_os_semaphore_destroy(acc_os_semaphore_t sem);

/**
 * @brief Convert value from network to host byte order
 *
 * @return The converted value
 */
extern uint16_t acc_os_ntohs(uint16_t value);

/**
 * @brief Convert value from host to network byte order
 *
 * @return The converted value
 */
extern uint16_t acc_os_htons(uint16_t value);

/**
 * @brief Convert value from network to host byte order
 *
 * @return The converted value
 */
extern uint32_t acc_os_ntohl(uint32_t value);

/**
 * @brief Convert value from host to network byte order
 *
 * @return The converted value
 */
extern uint32_t acc_os_htonl(uint32_t value);


#ifdef __cplusplus
}
#endif

#endif
