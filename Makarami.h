#include "Interpolators.h"
#include "Phasor.h"
#include <stk/Stk.h>
#include <stk/FileRead.h>

class Makarami
{
public:
	Makarami( unsigned int fs )
	{
		_fs = fs;
		stk::Stk::setSampleRate( fs );
		lookup = new stk::FileRead( "stk/rawwaves/sinewave.raw", true );
		if( lookup-> isOpen() )
		{
			buffer = new stk::StkFrames;
			buffer->resize( lookup->fileSize(), lookup->channels() );
			buffer->setDataRate( lookup->fileRate() );
			lookup->read( *buffer );
		}	
	}

	~Makarami()
	{
		if( lookup ) { delete lookup; lookup = nullptr; }
		if( buffer ) { delete buffer; buffer = nullptr; }
		if( playback ) { delete playback; playback = nullptr; }
	}

	double tick()
	{
		double index = playback->tick() * ( (double)buffer->frames() - 1 );
		if( index >= buffer->frames() - 1 ) 
			return buffer->interpolate( buffer->frames() - 1 );
		else if( index == 0 ) 
			return buffer->interpolate( 0 );
		unsigned int intdex = static_cast<unsigned int>( index );
		double fractional = index - intdex;
		double out = buffer->interpolate( intdex );
		switch( myType )
		{
			case 0:
				break;
			case 1:
				out = lerp( buffer->interpolate( intdex ), buffer->interpolate( intdex + 1 ), fractional );
				break;
			case 2:
				out = allpass( buffer->interpolate( intdex ), buffer->interpolate( intdex + 1 ), lastOut, fractional );
				break;
			default:
				break;
		}		
		lastOut = out;
		return out;
	}

	unsigned int type( unsigned int n_type )
	{
		if( n_type < 2 )
			myType = n_type;
		else 
			myType = 1;
		return myType;
	}

	unsigned int type() { return myType; }
	
private:
	unsigned int _fs = 0;
	unsigned int myType = 1;
	double lastOut;
	stk::FileRead* lookup = nullptr;
	stk::StkFrames* buffer = nullptr;
	Phasor* playback = nullptr;
};
