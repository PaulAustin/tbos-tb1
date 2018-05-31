/* basic classes for working with 'smart values. Not that smart really but a bit of
 * a standalone behaviour
 *
 *
 */


#ifndef VALUE_H_
#define VALUE_H_

#define COUNT_OF(x) ((int)(sizeof(x)/sizeof(0[x])))

// Async Values - designed to be set in an ISR and read by a fore ground process
// In the ISR there is always a 'value' set. The symantics of setting that value
/*
Copyright (c) 2018 Trashbots, Inc. - SDG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


class ATrigger {
protected:
	int _aValue;
public:
	ATrigger() {
		_aValue = 0;
	}
	virtual ~ATrigger() {};
	virtual void ASet(int) {
		// Ignore the value, just mark as set.
		_aValue = true;
	}
	virtual bool HasAsyncSet() {
		bool n = _aValue;
		_aValue = false;
		return n;
	}
	virtual void Reset() {
		_aValue = false;
	}
};

class AValue : ATrigger {
protected:
	int _value;
public:
	AValue() {
		_value = 0;
	}
	virtual ~AValue() {};

	virtual void ASet(int value) {
		_aValue = value;
	}
	virtual void Set(int value) {
		_aValue = value;
		_value = value;
	}
	virtual int  Get() {
		return _value;
	}
	virtual bool HasAsyncSet() {
		bool n = (_value != _aValue);
		_value = _aValue;
		return n;
	}
	virtual void Reset() {
		_value = _aValue;
	}
};

class SlewValue : AValue {
};
class LinMapValue : AValue {
};
class OscValue : AValue {
};
class PIPValue : AValue {
};

// EVF8  Escaped value format 8 byte chunks.
enum {
	// primary range is +/- 100 works for percents and ints.
	EVF8_Zero = 0,
	EVT8_IntMiniMax = 100,
	EVT8_IntMiniMin = -100,

	// Explicitly boolean values
	EVT8_True  = 101,		// boolean TRUE
	EVT8_False = 102,		// boolean FALSE

	// Special values
	EVT8_NULL  = 103,		// Not-a-value, empty set etc.
	EVT8_NaN,				// Not a real Number
	EVT8_PInf,				// Positive Infinity
	EVT8_NInf,				// Negative Infinity


	EVT8_Query	= 110,
	EVT8_Dot,				// For dotted notation, context specific
	EVT8_Eval,				// For dotted notation, context specific
	EVT8_Bind,				// For dotted notation, context specific

	// Special values below this are still single byte values.
	EVT8_MultiByteBase = 115,

	// Multi-byte encodings
	// Simple integers
	EVT8_Int8  = 115,		// one byte following
	EVT8_Int16,				// two bytes following
	EVT8_Int32,				// two bytes following
	EVT8_Float32,			// two bytes following
	EVT8_Int64,				// four bytes following
	EVT8_Float64,			// four bytes following

	// Null-byte terminated String
	EVT8_UTF8  = 121,		// null terminated UTF8 string
	// Counted Array ( all should be alike )
	EVT8_EltList  = 122,	// counted list of elements
	EVT8_NVPList  = 123,	// counted list of name value pairs
};

/* Read and build ale a byte at a time. */
class ValueReader {
private:
public:
	int _v;
	int _out;
	int _bytesRemaining;

	// Add reader proc for specific packet types?
	// this replaces the a switch with a function call
	// the sate proc can be
	// in FPG it could be parallel logic all looking for finish clause.
public:
	ValueReader() {
		Reset();
	}
	bool ReadV8(int ev8) {
		if (_bytesRemaining == 0) {
			if (ev8 >= EVT8_IntMiniMin && ev8 <= EVT8_MultiByteBase) {
				_v = ev8;
				return true;
			} else if (ev8 == EVT8_Int16) {
				_v = 0;
				_bytesRemaining = 2;
				return false;
			} else if (ev8 == EVT8_Int32) {
				_v = ev8;
			} else if (ev8 == EVT8_UTF8) {
				_v = ev8;
			}
		} else if (_bytesRemaining > 0) {
			_v = (_v << 8) | (ev8 & 0x00ff);
			_bytesRemaining--;
			return (_bytesRemaining == 0);
		}
		return false;
	}
	int Value() { return _v; }
	void Reset() {
		_v = _bytesRemaining = 0;
	}
};



#endif // VALUE_H_
