
#include "Value.h"

class SoundManager {
private:
	Value noteTempo;
	Value noteLength;
	Value noteSolfege;
	Value noteHertz;
	Value noteStatus;

public:
	void Init();
	void Run();
	void PluckFrequency(int f);
};


extern SoundManager gSound;
