#pragma once
#include <memory>
#include <iostream>
#define TESTPRINT
//typedef long long _DefaultPointerDatatype;
typedef unsigned int _DefaultPointerDatatype; //10^(4,294,967,295) is the maximum value
constexpr unsigned char bitwise00002222 = 1 + 2 + 2 * 2 + 2 * 2 * 2;
constexpr unsigned char bitwise22220000 = bitwise00002222 << 4;

template<typename A, typename B>
 void _inline bitwiseset(A* i, B pos, bool val) {
	if (val)
		*i |= (A)(1 << pos);
	else
		*i &= (A)~(1 << pos);
}

template<typename A, typename B>
 bool __forceinline bitwiseget(A i, B pos) {
	return ((i) & ((A)(1 << pos)));
}
 void _inline bitwiseswitch(unsigned char* i, unsigned char pos) {
	bitwiseset(i,pos, bitwiseget(*i,pos));
}

 unsigned char __forceinline bitwisereadlow(unsigned char i) {
	return (i & (bitwise22220000)) >> 4;
}
 unsigned char __forceinline bitwisereadhigh(unsigned char i) {
	return i & bitwise00002222;
}
 unsigned char __forceinline bitwisereadlowptr(unsigned char* i) {
	 return ((*i) & (bitwise22220000)) >> 4;
 }
 unsigned char __forceinline bitwisereadhighptr(unsigned char* i) {
	 return (*i) & bitwise00002222;
 }
 unsigned char __forceinline bitwisewritelow(unsigned char i, unsigned char val) {
	return i | (val << 4);
}
 unsigned char __forceinline bitwisewritehigh(unsigned char i, unsigned char val) {
	return i | val;
}
 
 void __forceinline bitwisewritelowptr(unsigned char* i, unsigned char val) {
	 *i &= bitwise00002222;
	*i |= (val << 4);
}
 void __forceinline bitwisewritehighptr(unsigned char* i, unsigned char val) {
	 *i &= bitwise22220000;
	*i |= val;
}


class LargeInt;
struct LargeInt_t { 
	//note for large integer one
	unsigned char* Data; //4 bytes in x86
	LargeInt* Owner; //4 bytes in x86
	_DefaultPointerDatatype Information; //64 bytes in x86
	//information, first bit is the sign, the rest of the bits is the SIZE OF ARRAY Data, however remember that each array value can hold 2 places
};


class LargeInt {
public:
	LargeInt() { 
		Internal = new LargeInt_t;
		Internal->Owner = nullptr;
		Internal->Data = (unsigned char*)malloc(1);
		if (!Internal->Data)
			return;

		Internal->Owner = this;
		Internal->Information = 1;
		Internal->Information <<= 1;
		memset(Internal->Data, NULL, 1);
	}
	~LargeInt() { 
		if (Internal->Owner == this)
			free(Internal->Data);

		if (Internal) 
			delete Internal;
	}
	bool Clean() {
		double Clear = 0.0;
		_DefaultPointerDatatype Size = (_DefaultPointerDatatype)(Internal->Information >> 1);
		for (unsigned int i = Size * 2; i > 0; i--) { //highest digit to smallest

			if (GetPlace(i) == 0) 
				Clear += 0.5;
			else
				break;
		}

		if (Clear == 0)
			return true;

		Clear = floor(Clear);
			
		Internal->Data = (unsigned char*)realloc(Internal->Data, Size-Clear);

		if (!Internal->Data)
			return false;

		bool prev = bitwiseget(Internal->Information, (unsigned char)0);
		Internal->Information = Size-Clear;

		Internal->Information <<= 1;
		bitwiseset(&Internal->Information, (unsigned char)0, prev);

		return true;

	}
	void operator=(const LargeInt& other) {
		if (!Internal->Data)
			return;

		_DefaultPointerDatatype Size = (_DefaultPointerDatatype)(other.Internal->Information >> 1);
	
		Internal->Data = (unsigned char*)realloc(Internal->Data, Size);
	

		if (!Internal->Data)
			return;

		memcpy(Internal->Data, other.Internal->Data, Size);
		Internal->Information = other.Internal->Information;
	}
	void operator=(const LargeInt* other) {

		other->Internal->Owner = this;
		Internal->Data = other->Internal->Data;
		Internal->Information = other->Internal->Information;
		Internal->Owner = this;
		
	}
	void operator+=(const LargeInt& other) {
		//toadd
		if (!Internal->Data || !other.Internal->Data)
			return;

		if (!IsNegative() && !IsNegative()) {
			//do add
			_DefaultPointerDatatype SizeThis = (_DefaultPointerDatatype)(Internal->Information >> 1);
			_DefaultPointerDatatype SizeOther = (_DefaultPointerDatatype)(other.Internal->Information >> 1);
			_DefaultPointerDatatype Largest = SizeThis;
			
			if (SizeOther > Largest)
				Largest = SizeOther; //we have the largest size, of which an addition will yield this + 1 most likely

			if (Largest + 1 != SizeThis) {
				Internal->Data = (unsigned char*)realloc(Internal->Data, Largest + 1); //just not clean it then hiya

				if (!Internal->Data)
					return;
			
				memset(&Internal->Data[SizeThis], 0, Largest + 1 - SizeThis);

				ResizeFPlace(Largest+1);

			}

			
			//TO DO: ADD CLEANING OF NEW VALUES TO ZERO
			// 2^7 2^6 2^5 2^4 2^3 2^2 2^1 2^0 -> 128 64 32 16 8 4 2 1 -> 8 4 2 1   8 4 2 1 (max is 15 per 4 bits)
		//	unsigned char buf[3] = { 0,0,0 }; //
			unsigned char Buffer[3] = {0,0, 0};
			for (int i = Largest; i > -1; i--) {
				//ADD NEW ADD FUNC
				
				if (i + 1 > SizeOther) 
					continue;
				

				if (i > SizeThis) {
					//deal with low n high
					bitwisewritelowptr(&Internal->Data[i], bitwisereadlow(other.Internal->Data[i]));
					bitwisewritehighptr(&Internal->Data[i], bitwisereadhigh(other.Internal->Data[i]));
					continue;
				}
				if (Internal->Data[i] == other.Internal->Data[i] && Internal->Data[i] == 0) //nothing to add {
					continue;
				
				//write from the cur data point
				Buffer[0] = bitwisereadlow(Internal->Data[i]) + bitwisereadlow(other.Internal->Data[i]);
				Buffer[1] = bitwisereadhigh(Internal->Data[i]) + bitwisereadhigh(other.Internal->Data[i]);

				if (Buffer[1] > 9) {
					Buffer[0]++;
					Buffer[1] -= 10;
				}

				
				while (Buffer[0] > 9) {
					Buffer[2]++;
					Buffer[0] -= 10;
				}
				if(i != Largest)
					bitwisewritehighptr(&Internal->Data[i + 1], Internal->Data[i + 1] + Buffer[2]);

				bitwisewritelowptr(&Internal->Data[i], Buffer[0]);
				bitwisewritehighptr(&Internal->Data[i], Buffer[1]);
			}

			return;
		}
	}
	bool operator==( LargeInt& other) {
		//toadd
		if (IsNegative() != other.IsNegative())
			return false;

		_DefaultPointerDatatype SizeThis = (_DefaultPointerDatatype)(Internal->Information >> 1);
		_DefaultPointerDatatype SizeOther = (_DefaultPointerDatatype)(other.Internal->Information >> 1);
		_DefaultPointerDatatype Largest = SizeThis;
		if (SizeOther > Largest)
			Largest = SizeOther;

	
		return true;
		
	}
	bool __forceinline IsNegative() const {
		return bitwiseget(Internal->Information, (unsigned char)0);
	}
	void SetNegative(bool state) const { //true = negative
		bitwiseset(&Internal->Information, (unsigned char)0, state);
	}
	unsigned char GetPlace(_DefaultPointerDatatype place) {
		if (!Internal->Data || place < 1)
			return 0;

		//dynamic allocation

		double placediv2 = (double)place * 0.5;
		place = round(placediv2);


		if (place > (_DefaultPointerDatatype)(Internal->Information >> 1)) 
			return 0;
		
		

		if (placediv2 == place)  //whole number
			return bitwisereadlow(Internal->Data[place-1]);


		return bitwisereadhigh(Internal->Data[place - 1]);

	}
	void SetPlace(unsigned char number, _DefaultPointerDatatype place) {

		if (number > 9 || !Internal->Data || place < 1)
			return;

		//dynamic allocation

		double placediv2 = (double)place * 0.5;
		place = round(placediv2);

		if (place > (_DefaultPointerDatatype)(Internal->Information >> 1)) {
		
			Internal->Data = (unsigned char*)realloc(Internal->Data, place);
			if (!Internal->Data)
				return;

			_DefaultPointerDatatype m = (_DefaultPointerDatatype)(Internal->Information >> 1);
			memset(&Internal->Data[m], 0, place - m);
		}

	

		ResizeFPlace(place);


		//now we setup the place
		if (placediv2 == place)  //whole number
			return bitwisewritelowptr(&Internal->Data[place - 1], number);
		

		return bitwisewritehighptr(&Internal->Data[place - 1], number);

	}


	LargeInt_t* Internal;

	private:
	void ResizeFPlace(_DefaultPointerDatatype nplace) {
		bool prev = bitwiseget(Internal->Information, (unsigned char)0);
		Internal->Information = nplace;
		Internal->Information <<= 1;
		bitwiseset(&Internal->Information, (unsigned char)0, prev);
	}
};
#ifdef TESTPRINT
#include <string>
 void _inline printint( LargeInt& a) {
	if (!a.Internal)
		return;

	if (a.IsNegative())
		std::cout << "-";


	for (unsigned int i = (_DefaultPointerDatatype)(a.Internal->Information >> 1) * 2; i > 0; i--) {
		//std::cout << "place:" << i << "\n";
		std::cout << (unsigned int)a.GetPlace(i);
	}
}
 int _inline toint(LargeInt& a) {
	 if (!a.Internal)
		 return 0;
	 int ans = 0;
	


	 for (unsigned int i = (_DefaultPointerDatatype)(a.Internal->Information >> 1) * 2; i > 0; i--) {
		
		ans += a.GetPlace(i) * pow(10, i - 1);
	 }
	 if (a.IsNegative())
		 ans *= -1;
	 return ans;
 }
#endif
