
#include "Value.h"

class SoundManager {
private:
	AValue noteTempo;
	AValue noteLength;
	AValue noteSolfege;
	AValue noteHertz;
	AValue noteStatus;
	int _noteTimeRemaining;

public:
	void Init();
	void Run();
	void PluckFrequency(int f);
};


extern SoundManager gSound;
