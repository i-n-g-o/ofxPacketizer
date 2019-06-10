/*//-------------------------------------------------------------------------------
*	Packetizer.cpp
*
*	Packetizer to analyze data for start and end condition
*	https://github.com/i-n-g-o/Packetizer
*	
*	written by: Ingo Randolf - 2014
*
*
*	This library is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
//-------------------------------------------------------------------------------*/


#include "Packetizer.h"


//-------------------------------------------------------------------------------
// Constructor //////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
Packetizer::Packetizer() :
	m_buffer()
	,m_bufferSize(0)
	,m_startCondition(0)
	,m_startConditionSize(0)
	,m_endCondition(0)
	,m_endConditionSize(0)
{
	initVars();
	// init with default size of 32
	init(32);
}

Packetizer::Packetizer(size_t _size) :
	m_buffer()
	,m_bufferSize(0)
	,m_startCondition(0)
	,m_startConditionSize(0)
	,m_endCondition(0)
	,m_endConditionSize(0)
{
	initVars();
	init(_size);
}


Packetizer::~Packetizer()
{
	freeBuffer(&m_buffer, &m_bufferSize);
	freeBuffer(&m_startCondition, &m_startConditionSize);
	freeBuffer(&m_endCondition, &m_endConditionSize);
}


//-------------------------------------------------------------------------------
// Public Methods ///////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------

uint8_t Packetizer::init(size_t _size)
{
	return setBufferSize(_size);
}

uint8_t Packetizer::setBufferSize(size_t _size)
{
	// free buffer
	freeBuffer(&m_buffer, &m_bufferSize);
	
	// allocate buffer
	uint8_t result = allocateBuffer(&m_buffer, &m_bufferSize, _size);

	// reset things, erase buffer
	clear();
	
	return result;
}


void Packetizer::clear()
{
	m_packetEnd = -1;
	m_index = 0;
	m_startIndex = 0;
	m_endIndex = 0;
	m_startFound = false;
	
	if (m_buffer == 0) return;
	if (m_bufferSize == 0) return;
	
	// clear buffer
	memset(m_buffer, 0, m_bufferSize);
}

/*
*	append data
*/
uint8_t Packetizer::appendData(char _c)
{
	return appendData((uint8_t)_c);
}

uint8_t Packetizer::appendData(int _data)
{
	return appendData((uint8_t*)&_data, sizeof(int));
}

uint8_t Packetizer::appendData(long _data)
{
	return appendData((uint8_t*)&_data, sizeof(long));
}

uint8_t Packetizer::appendData(const char* _data)
{
	return appendData((uint8_t*) _data, strlen(_data));
}

uint8_t Packetizer::appendData(string _data)
{
#ifdef __RFduino__
	// Stupidly, RFDuino's copy of string uses cstr() instead of c_str()
	return appendData((uint8_t*)_data.cstr(), (size_t)_data.length());
#else
	return appendData((uint8_t*)_data.c_str(), (size_t)_data.length());
#endif
}

// append data
uint8_t Packetizer::appendData(uint8_t* _buffer, size_t _bufferSize)
{
	if (m_buffer == 0) return pz_noBuffer;
	if (_buffer == 0) return pz_noBuffer;
	if (_bufferSize == 0) return pz_bufferSize;
	
	
	unsigned int i;
	for (i=0; i<_bufferSize; i++)
	{
		appendData(_buffer[i]);
	}
	
	return pz_noErr;
}

// append one byte and test conditions
uint8_t Packetizer::appendData(uint8_t _c)
{
	// safety
	if (m_buffer == 0) return pz_noBuffer;
	
	m_packetEnd = -1;
	
	if (m_startConditionSize > 0)
	{
		// search for start...
		if (_c != m_startCondition[m_startIndex])
			m_startIndex = 0;
			
		if (_c == m_startCondition[m_startIndex])
		{
			if(++m_startIndex >= m_startConditionSize)
			{
				// startcondition found
				// we always start at index 0
				m_index = 0;
				m_endIndex = 0;
				m_startIndex = 0;				
				m_startFound = true;
				
				_packetStart();
				
				// dont add caracter to buffer
				return pz_noErr;
			}
		}
		
		
		if (!m_startFound) return pz_noErr;
	}
	

	// add data to our buffer
	m_buffer[m_index] = _c;
	
	
	//------------------------------------------------
	// start was found or no startcondition was set
	
	if (m_endConditionSize > 0)
	{	
		// look for endcondition...		
		if (_c != m_endCondition[m_endIndex])
			m_endIndex = 0;
		
		
		if (_c == m_endCondition[m_endIndex])
		{
			if(++m_endIndex >= m_endConditionSize)
			{
				// we found an end
				m_packetEnd = 0;
				
				// calculate len only if it will be >0
				if ( m_index >= m_endConditionSize) {
					m_packetEnd = m_index + 1 - m_endConditionSize;
				}
				
				_packetFound(m_buffer, m_packetEnd);
				
				// reset index
				m_index = 0;
				m_endIndex = 0;
				m_startIndex = 0;
				m_startFound = false;
			
				return pz_noErr;
			}
		}
	}
	

	// increment writing index
	if (++m_index >= m_bufferSize)
	{
		// reset index	
		m_index = 0;
		
		_packetOverflow(m_buffer, m_bufferSize);
	}
	
	return pz_noErr;	
}


/*
*	set startcondition
*/
uint8_t Packetizer::setStartCondition(char _data)
{
	return setStartCondition((uint8_t)_data);
}

uint8_t Packetizer::setStartCondition(uint8_t _data)
{
	return setStartCondition((uint8_t*)&_data, sizeof(uint8_t));
}

uint8_t Packetizer::setStartCondition(int _data)
{
	return setStartCondition((uint8_t*)&_data, sizeof(int));
}

uint8_t Packetizer::setStartCondition(long _data)
{
	return setStartCondition((uint8_t*)&_data, sizeof(long));
}

uint8_t Packetizer::setStartCondition(const char* _data)
{
	return setStartCondition((uint8_t*)_data, strlen(_data));
}

uint8_t Packetizer::setStartCondition(string _data)
{
#ifdef __RFduino__
	// Stupidly, RFDuino's copy of string uses cstr() instead of c_str()
	return setStartCondition((uint8_t*)_data.cstr(), (size_t)_data.length());
#else
	return setStartCondition((uint8_t*)_data.c_str(), (size_t)_data.length());
#endif
}

uint8_t Packetizer::setStartCondition(uint8_t* _buffer, size_t _bufferSize)
{
	// free buffer
	freeBuffer(&m_startCondition, &m_startConditionSize);
	
	// reset
	m_startIndex = 0;
	m_startFound = false;
	
	// safety
	if (_buffer == 0) return pz_noErr;
	if (_bufferSize == 0) return pz_noErr;

	// allocate buffer
	uint8_t result = allocateBuffer(&m_startCondition, &m_startConditionSize, _bufferSize);	
	if (result != pz_noErr)
	{
		return result;
	}
	
	// copy bytes
	memcpy(m_startCondition, _buffer, _bufferSize);
	
	return pz_noErr;
}


/*
*	set endcondition
*/
uint8_t Packetizer::setEndCondition(char _data)
{
	return setEndCondition((uint8_t) _data);
}

uint8_t Packetizer::setEndCondition(uint8_t _data)
{
	return setEndCondition((uint8_t*)&_data, sizeof(uint8_t));
}

uint8_t Packetizer::setEndCondition(int _data)
{
	return setEndCondition((uint8_t*)&_data, sizeof(int));
}

uint8_t Packetizer::setEndCondition(long _data)
{
	return setEndCondition((uint8_t*)&_data, sizeof(long));
}

uint8_t Packetizer::setEndCondition(const char* _data)
{
	return setEndCondition((uint8_t*)_data, strlen(_data));
}

uint8_t Packetizer::setEndCondition(string _data)
{
#ifdef __RFduino__
	// Stupidly, RFDuino's copy of string uses cstr() instead of c_str()
	return setEndCondition((uint8_t*)_data.cstr(), (size_t)_data.length());
#else
	return setEndCondition((uint8_t*)_data.c_str(), (size_t)_data.length());
#endif
}

uint8_t Packetizer::setEndCondition(uint8_t* _buffer, size_t _bufferSize)
{
	// free end condition
	freeBuffer(&m_endCondition, &m_endConditionSize);
	// reset
	m_endIndex = 0;
	
	// safety
	if (_buffer == 0) return pz_noErr;
	if (_bufferSize == 0) return pz_noErr;
	
	
	// allocate buffer
	uint8_t result = allocateBuffer(&m_endCondition, &m_endConditionSize, _bufferSize);	
	if (result != pz_noErr)
	{
		return result;
	}
	
	// copy bytes
	memcpy(m_endCondition, _buffer, _bufferSize);

	return pz_noErr;
}


//-------------------------------------------------------------------------------
// send start and end condition
//-------------------------------------------------------------------------------
#ifdef ARDUINO
void Packetizer::sendStartCondition(Print& _print)
{
    if (m_startConditionSize > 0) {
        _print.write(m_startCondition, m_startConditionSize);
    }
}

void Packetizer::sendEndCondition(Print& _print)
{
    if (m_endConditionSize > 0) {
        _print.write(m_endCondition, m_endConditionSize);
    }
}
#endif


//-------------------------------------------------------------------------------
// internal
//-------------------------------------------------------------------------------
void Packetizer::_packetStart()
{
	if (user_onStart) {
		user_onStart();
	}

	if (listener != 0) {
		listener->onPacketStart();
	}
}

void Packetizer::_packetFound(uint8_t* buffer, size_t size)
{
	// call user-method if any
	if (user_onPacket) {
		user_onPacket(buffer, size);
	}

	if (listener != 0) {
		listener->onPacket(buffer, size);
	}
}

void Packetizer::_packetOverflow(uint8_t* buffer, size_t size)
{
	//call overflow...
	if (user_onOverflow) {
		user_onOverflow(buffer, size);
	}
	
	if (listener != 0) {
		listener->onPacketOverflow(buffer, size);
	}
}


//-------------------------------------------------------------------------------
// listener
//-------------------------------------------------------------------------------
void Packetizer::setListener(PacketizerCb& newListener)
{
	listener = &newListener;
}

void Packetizer::removeListener()
{
	listener = 0;
}

//-------------------------------------------------------------------------------
// Callback functions
//-------------------------------------------------------------------------------
void Packetizer::onPacketStart( void (*function)(void) )
{
	user_onStart = function;
}

void Packetizer::onPacket( void (*function)(uint8_t*, size_t) )
{
	user_onPacket = function;
}

void Packetizer::onOverflow( void (*function)(uint8_t*, size_t) )
{
	user_onOverflow = function;
}


//-------------------------------------------------------------------------------
// Private Methods //////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
void Packetizer::initVars()
{

	m_index = 0;
	m_startFound = false;
	m_startIndex = 0;
	m_endIndex = 0;
	m_packetEnd = -1;
	
	// internal buffer
	if (m_buffer != 0) {
		freeBuffer(&m_buffer, &m_bufferSize);
	}
	m_buffer = 0;
	m_bufferSize = 0;
	
	if (m_startCondition != 0) {
		freeBuffer(&m_startCondition, &m_startConditionSize);
	}
	m_startCondition = 0;
	m_startConditionSize = 0;
	
	if (m_endCondition != 0) {
		freeBuffer(&m_endCondition, &m_endConditionSize);
	}
	m_endCondition = 0;
	m_endConditionSize = 0;
	
	// callbacks
	listener = 0;
	
	user_onStart = 0;
	user_onPacket = 0;
	user_onOverflow = 0;
}


void Packetizer::freeBuffer(uint8_t** _buffer, size_t* _bufferSize)
{
	// free buffer if allocated
	if (*_buffer != 0) {
		free(*_buffer);		
	}
	
	*_buffer = 0;
	*_bufferSize = 0;
}


uint8_t Packetizer::allocateBuffer(uint8_t** _buffer, size_t* _bufferSize, size_t _size)
{
	// safety
	if (_size == 0) return pz_bufferSize;
		
	
	// allocate buffer
	*_buffer = (uint8_t*)malloc(_size);
  
  	// test
	if (*_buffer == 0) {
		return pz_noBuffer;
	}
	
	// set size
	*_bufferSize = _size;
	
	return pz_noErr;
}
