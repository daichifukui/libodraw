/*
 * Storage media buffer
 *
 * Copyright (C) 2010-2022, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <memory.h>

#include "odrawtools_libcerror.h"
#include "storage_media_buffer.h"

/* Creates a storage media buffer
 * Make sure the value buffer is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int storage_media_buffer_initialize(
     storage_media_buffer_t **buffer,
     size_t size,
     libcerror_error_t **error )
{
	static char *function  = "storage_media_buffer_initialize";
	size_t raw_buffer_size = 0;

	if( buffer == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid buffer.",
		 function );

		return( -1 );
	}
	if( *buffer != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid buffer value already set.",
		 function );

		return( -1 );
	}
	if( size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid buffer size value exceeds maximum.",
		 function );

		return( -1 );
	}
	*buffer = memory_allocate_structure(
	           storage_media_buffer_t );

	if( *buffer == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create buffer.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *buffer,
	     0,
	     sizeof( storage_media_buffer_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear buffer.",
		 function );

		memory_free(
		 *buffer );

		*buffer = NULL;

		return( -1 );
	}
	if( size > 0 )
	{
		raw_buffer_size = size;

/* TODO can low level functions and direct IO be combined ? */
#if defined( memory_allocate_aligned )
		if( memory_allocate_aligned(
		     (void **) &( ( *buffer )->raw_buffer ),
		     raw_buffer_size,
		     512 ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create aligned raw buffer.",
			 function );

			goto on_error;
		}
#else
		( *buffer )->raw_buffer = (uint8_t *) memory_allocate(
		                                       sizeof( uint8_t ) * raw_buffer_size );
			
		if( ( *buffer )->raw_buffer == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create raw buffer.",
			 function );

			goto on_error;
		}
#endif /* defined( memory_allocate_aligned ) */

		( *buffer )->raw_buffer_size = size;
	}
	return( 1 );

on_error:
	if( *buffer != NULL )
	{
		if( ( *buffer )->raw_buffer != NULL )
		{
			memory_free(
			 ( *buffer )->raw_buffer );
		}
		memory_free(
		 *buffer );

		*buffer = NULL;
	}
	return( -1 );
}

/* Frees a storage media buffer
 * Returns 1 if successful or -1 on error
 */
int storage_media_buffer_free(
     storage_media_buffer_t **buffer,
     libcerror_error_t **error )
{
	static char *function = "storage_media_buffer_free";

	if( buffer == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid buffer.",
		 function );

		return( -1 );
	}
	if( *buffer != NULL )
	{
		if( ( *buffer )->raw_buffer != NULL )
		{
			memory_free(
			 ( *buffer )->raw_buffer );
		}
		memory_free(
		 *buffer );

		*buffer = NULL;
	}
	return( 1 );
}

/* Resizes a buffer
 * Returns 1 if successful or -1 on error
 */
int storage_media_buffer_resize(
     storage_media_buffer_t *buffer,
     size_t size,
     libcerror_error_t **error )
{
	void *reallocation    = NULL;
	static char *function = "storage_media_buffer_resize";

	if( buffer == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid buffer.",
		 function );

		return( -1 );
	}
	if( size > buffer->raw_buffer_size )
	{
		reallocation = memory_reallocate(
				buffer->raw_buffer,
				sizeof( uint8_t ) * size );

		if( reallocation == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to reallocate read buffer.",
			 function );

			return( -1 );
		}
		buffer->raw_buffer           = (uint8_t *) reallocation;
		buffer->raw_buffer_size      = size;
		buffer->raw_buffer_data_size = 0;
	}
	return( 1 );
}

/* Retrieves the reference to the actual data and its size
 * Returns 1 if successful or -1 on error
 */
int storage_media_buffer_get_data(
     storage_media_buffer_t *buffer,
     uint8_t **data,
     size_t *data_size,
     libcerror_error_t **error )
{
	static char *function = "storage_media_buffer_get_data";

	if( buffer == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid buffer.",
		 function );

		return( -1 );
	}
	if( data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data.",
		 function );

		return( -1 );
	}
	if( data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	*data      = buffer->raw_buffer;
	*data_size = buffer->raw_buffer_data_size;

	return( 1 );
}

