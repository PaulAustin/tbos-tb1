
#include "Value.h"

class SoundManager {
private:
	Value noteTempo;
	Value noteLength;
	Value noteSolfege;
	Value noteHertz;
	Value noteStatus;

	int _noteTimeRemaining = 0;

public:
	void Init();
	void Run();
	void PluckFrequency(int f);
};


extern SoundManager gSound;
