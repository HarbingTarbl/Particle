#ifndef _PARTICLE_UTILITIES_H_
#define _PARTICLE_UTILITIES_H_

namespace particle
{
	void CheckError(const char* str = nullptr);


#define __STRIG(x) #x
#define _STRIG(x) __STRIG(x)
#ifdef _DEBUG
#define CHERRL(CMD) CMD; CheckError(_STRIG(CMD) "@" __FUNCTION__ ":" _STRIG(__LINE__))
#else
#define CHERRL(CMD) CMD
#endif



};

#endif