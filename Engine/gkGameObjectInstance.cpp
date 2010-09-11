/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): none yet.
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
#include "gkGameObjectInstance.h"
#include "gkGameObjectGroup.h"
#include "gkGameObject.h"
#include "gkScene.h"
#include "gkUtils.h"
#include "gkLogger.h"
#include "gkValue.h"




gkGameObjectInstance::gkGameObjectInstance(gkGameObjectGroup* group, UTsize uid)
	:   m_id(uid),
	    m_parent(group),
	    m_firstLoad(true),
		m_isInstanced(false)
{
}



gkGameObjectInstance::~gkGameObjectInstance()
{
	Objects::Iterator iter = m_objects.iterator();
	while (iter.hasMoreElements())
	{
		gkGameObject* gobj = iter.getNext().second;
		GK_ASSERT(!gobj->isInstanced());
		delete gobj;
	}

	m_objects.clear();
}



void gkGameObjectInstance::addObject(gkGameObject* gobj)
{
	if (!gobj)
		return;

	const gkHashedString name = m_parent->getResourceName().str() + "/" + gobj->getName() + "/Handle_" +  gkToString((int)m_id);


	if (m_objects.find(name) != UT_NPOS)
	{
		gkLogMessage("GameObjectInstance: Duplicate object " << name.str() << ".");
		return;
	}


	gkGameObject* ngobj = gobj->clone(name.str());
	ngobj->setOwner(0);


	m_objects.insert(name, ngobj);


	// Lightly attach
	ngobj->_makeGroupInstance(this);


	ngobj->setActiveLayer(true);
	ngobj->setLayer(0xFFFFFFFF);
}




gkGameObject* gkGameObjectInstance::getObject(const gkHashedString& name)
{
	UTsize pos;
	if ((pos = m_objects.find(name)) == UT_NPOS)
	{
		gkLogMessage("GameObjectInstance: Missing object " << name.str() << ".");
		return 0;
	}
	return m_objects.at(pos);
}


void gkGameObjectInstance::destroyObject(gkGameObject* gobj)
{
	if (!gobj)
		return;

	if (gobj->getGroupInstance() != this)
	{
		gkLogMessage("GameObjectInstance: Attempting to remove an object that does not belong to this instance!");
		return;
	}


	const gkHashedString name = gobj->getName();

	UTsize pos;
	if ((pos = m_objects.find(name)) == UT_NPOS)
	{
		gkLogMessage("GameObjectInstance: Missing object " << name.str() << ".");
		return;
	}

	m_objects.remove(name);
	delete gobj;
}




void gkGameObjectInstance::destroyObject(const gkHashedString& name)
{

	UTsize pos;
	if ((pos = m_objects.find(name)) == UT_NPOS)
	{
		gkLogMessage("GameObjectInstance: Missing object " << name.str() << ".");
		return;
	}

	gkGameObject* gobj = m_objects.at(pos);
	gobj->destroyInstance();


	m_objects.remove(name);
	delete gobj;
}




bool gkGameObjectInstance::hasObject(const gkHashedString& name)
{
	return m_objects.find(name) != UT_NPOS;
}



void gkGameObjectInstance::applyTransform(const gkTransformState& trans)
{
	const gkMatrix4 plocal = trans.toMatrix();


	Objects::Iterator iter = m_objects.iterator();
	while (iter.hasMoreElements())
	{
		gkGameObject* obj = iter.getNext().second;

		// Update transform relitave to owner
		gkGameObjectProperties& props = obj->getProperties();


		gkMatrix4 clocal;
		props.m_transform.toMatrix(clocal);

		// merge back to transform state
		props.m_transform = gkTransformState(plocal * clocal);
	}

}



bool gkGameObjectInstance::hasObject(gkGameObject* gobj)
{
	return gobj && m_objects.find(gobj->getName()) && gobj->getGroupInstance() == this;
}



void gkGameObjectInstance::cloneObjects(gkScene *scene, const gkTransformState& from, int time,
                                        const gkVector3& linearVelocity, bool tsLinLocal,
                                        const gkVector3& angularVelocity, bool tsAngLocal)
{
	if (!isInstanced())
		return;

	GK_ASSERT(scene);


	const gkMatrix4 plocal = from.toMatrix();


	Objects::Iterator iter = m_objects.iterator();
	while (iter.hasMoreElements())
	{
		gkGameObject* oobj = iter.getNext().second;
		gkGameObject* nobj = scene->cloneObject(oobj, time);



		// be sure this info was not cloned!
		GK_ASSERT(!nobj->isGroupInstance());


		// Update transform relitave to owner
		gkGameObjectProperties& props = nobj->getProperties();

		gkMatrix4 clocal;
		props.m_transform.toMatrix(clocal);

		// merge back to transform state
		props.m_transform = gkTransformState(plocal * clocal);


		nobj->createInstance();


		if (props.isRigidOrDynamic() || props.isGhost())
		{
			if (!linearVelocity.isZeroLength())
				nobj->setLinearVelocity(linearVelocity, tsLinLocal ? TRANSFORM_LOCAL : TRANSFORM_PARENT);
		}

		if (props.isRigid())
		{
			if (!angularVelocity.isZeroLength())
				nobj->setAngularVelocity(angularVelocity, tsAngLocal ? TRANSFORM_LOCAL : TRANSFORM_PARENT);
		}
	}

}

void gkGameObjectInstance::createObjectInstances(gkScene *scene)
{
	GK_ASSERT(scene);


	if (m_firstLoad)
	{
		applyTransform(m_transform);
		m_firstLoad = false;
	}

	Objects::Iterator iter = m_objects.iterator();
	while (iter.hasMoreElements())
	{
		gkGameObject *gobj = iter.getNext().second;

		gobj->setOwner(scene);

		gobj->createInstance();
	}

	m_isInstanced = true;
}



void gkGameObjectInstance::destroyObjectInstances(void)
{

	Objects::Iterator iter = m_objects.iterator();
	while (iter.hasMoreElements())
	{
		gkGameObject *gobj = iter.getNext().second;
		gobj->destroyInstance();
		gobj->setOwner(0);
	}

	m_isInstanced = false;
}
