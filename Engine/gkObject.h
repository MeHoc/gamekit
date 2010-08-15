/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Nestor Silveira.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#ifndef _gkObject_h_
#define _gkObject_h_

#include "gkCommon.h"
#include "gkHashedString.h"

class gkGameObject;

// Base class representing a loadable object
class gkObject
{
protected:
	const gkString      m_name;
	bool                m_loaded;

	virtual void preLoadImpl(void) {}
	virtual void preUnloadImpl(void) {}
	virtual void loadImpl(void) {}
	virtual void unloadImpl(void) {}
	virtual void postLoadImpl(void) {}
	virtual void postUnloadImpl(void) {}

public:

	gkObject(const gkString &name);
	virtual ~gkObject();

	void load(void);
	void unload(void);
	void reload(void);


	GK_INLINE const gkString &getName(void)        {return m_name;}
	GK_INLINE bool           isLoaded(void) const  {return m_loaded;}
};



#endif//_gkObject_h_
