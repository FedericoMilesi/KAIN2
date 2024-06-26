#include "CORE.H"
#include "OBTABLE.H"
#include "RAZIEL/RAZIEL.H"
#include "REAVER.H"
#include "GLYPH.H"
#include "PHYSOBS.H"
#include "MONSTER/MONAPI.H"
#include "FX.H"
#include "EVENT.H"
#include "GENERIC.H"
#include "GAMELOOP.H"
#include "SCRIPT.H"
#include "GEX2.H"
#include "Game/STREAM.H"
#include "MEMPACK.H"

struct ObjectAccess objectAccess[28] =
{
	{
		"hud_____",
		NULL
	},
	{
		"flame___",
		NULL
	},
	{
		"fonts___",
		NULL
	},
	{
		"waterfx_",
		NULL
	},
	{
		"dbgfont_",
		NULL
	},
	{
		"cammode_",
		NULL
	},
	{
		"introfx_",
		NULL
	},
	{
		"mapicon_",
		NULL
	},
	{
		"paths___",
		NULL
	},
	{
		"soul____",
		NULL
	},
	{
		"particle",
		NULL
	},
	{
		"knife___",
		NULL
	},
	{
		"stick___",
		NULL
	},
	{
		"xbow____",
		NULL
	},
	{
		"fthrow__",
		NULL
	},
	{
		"moregg__",
		NULL
	},
	{
		"wcegg___",
		NULL
	},
	{
		"eggsac__",
		NULL
	},
	{
		"mound___",
		NULL
	},
	{
		"force___",
		NULL
	},
	{
		"glphicon",
		NULL
	},
	{
		"healthu_",
		NULL
	},
	{
		"sreavr__",
		NULL
	},
	{
		"wrpface_",
		NULL
	},
	{
		"healths_",
		NULL
	},
	{
		"eaggot__",
		NULL
	},
	{
		"eaggots_",
		NULL
	},
	{
		NULL,
		NULL
	}
};

struct ObjectFunc objectFunc[8] =
{
	{
		"raziel__",
		RazielInit,
		RazielProcess,
		RazielCollide,
		RazielQuery,
		RazielPost,
		RazielAdditionalCollide,
		NULL,
		NULL
	},
	{
		"sreavr__",
		SoulReaverInit,
		SoulReaverProcess,
		SoulReaverCollide,
		SoulReaverQuery,
		SoulReaverPost,
		NULL,
		NULL,
		NULL
	},
	{
		"glphicon",
		GlyphInit,
		NULL,
		GlyphCollide,
		GlyphQuery,
		GlyphPost,
		NULL,
		NULL,
		NULL
	},
	{
		"physical",
		InitPhysicalObject,
		ProcessPhysicalObject,
		CollidePhysicalObject,
		PhysicalObjectQuery,
		PhysicalObjectPost,
		NULL,
		PhysicalRelocateTune,
		NULL
	},
	{
		"monster_",
		MonsterInit,
		MonsterProcess,
		MonsterCollide,
		MonsterQuery,
		MonsterMessage,
		MonsterAdditionalCollide,
		MonsterRelocateTune,
		MonsterRelocateInstanceObject
	},
	{
		"particle",
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		FX_RelocateGeneric,
		NULL
	},
	{
		"litshaft",
		LitShaftInit,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	}
};

void OBTABLE_InstanceInit(struct _Instance* instance) // Matching - 100%
{
	long id;

	id = instance->object->id;

	if (id < 0)
	{
		GenericInit(instance, &gameTrackerX);
	}
	else
	{
		if (objectFunc[id].initFunc != NULL)
		{
			objectFunc[id].initFunc(instance, &gameTrackerX);
		}
	}

	instance->flags2 |= 0x200000;

	if (!(instance->flags & 0x100000))
	{
		SCRIPT_InstanceSplineInit(instance);
	}

	if (instance->intro != NULL && (instance->intro->flags & 0x20))
	{
		instance->flags2 &= ~0x4;
		instance->flags2 &= ~0x20000;
	}
}

void OBTABLE_GetInstanceCollideFunc(struct _Instance* instance) // Matching - 100%
{
	long id;

	id = instance->object->id;

	if (id >= 0)
	{
		instance->collideFunc = objectFunc[id].collideFunc;
	}
	else
	{
		instance->collideFunc = GenericCollide;
	}
}

void OBTABLE_GetInstanceAdditionalCollideFunc(struct _Instance *instance) // Matching - 100%
{ 
	long id;

	id = instance->object->id;

	if (id >= 0)
	{
		instance->additionalCollideFunc = objectFunc[id].additionalCollideFunc;
	}
	else
	{
		instance->additionalCollideFunc = NULL;
	}
}

void OBTABLE_GetInstanceProcessFunc(struct _Instance* instance) // Matching - 100%
{
	long id;

	id = instance->object->id;

	if (id >= 0)
	{
		instance->processFunc = objectFunc[id].processFunc;
	}
	else
	{
		instance->processFunc = GenericProcess;
	}
}

void OBTABLE_GetInstanceQueryFunc(struct _Instance* instance) // Matching - 100%
{
	long id;

	id = instance->object->id;

	if (id >= 0)
	{
		instance->queryFunc = objectFunc[id].queryFunc;
	}
	else
	{
		instance->queryFunc = GenericQuery;
	}
}

void OBTABLE_GetInstanceMessageFunc(struct _Instance* instance) // Matching - 100%
{
	long id;

	id = instance->object->id;

	if (id >= 0)
	{
		instance->messageFunc = objectFunc[id].messageFunc;
	}
	else
	{
		instance->messageFunc = GenericMessage;
	}
}

void OBTABLE_InitObjectWithID(struct Object* object) // Matching - 100%
{
	long id;

	if (object != NULL)
	{
		struct ObjectAccess* oa;

		if ((object->oflags2 & 0x40000))
		{
			for (id = 0; (objectFunc[id].scriptName != NULL) && (strcmp(objectFunc[id].scriptName, "physical")); id++)
			{

			}
		}
		else if ((object->oflags2 & 0x80000))
		{
			for (id = 0; (objectFunc[id].scriptName != NULL) && (strcmp(objectFunc[id].scriptName, "monster_")); id++)
			{

			}
		}
		else
		{
			for (id = 0; (objectFunc[id].scriptName != NULL) && (strcmp(objectFunc[id].scriptName, object->script)); id++)
			{

			}
		}

		if (objectFunc[id].scriptName != NULL)
		{
			object->id = (short)id;
		}
		else
		{
			object->id = -1;
		}

		for (oa = objectAccess; oa->objectName != NULL; oa++)
		{
			if ((((unsigned int*)oa->objectName)[0] == ((unsigned int*)object->name)[0])
				&& (((unsigned int*)oa->objectName)[1] == ((unsigned int*)object->name)[1]))
			{
				oa->object = object;
				break;
			}
		}
	}
}

void OBTABLE_ClearObjectReferences() // Matching - 100%
{
	struct ObjectAccess* oa;

	for (oa = objectAccess; oa->objectName != NULL; oa++)
	{
		oa->object = NULL;
	}
}

void OBTABLE_RemoveObjectEntry(struct Object* object) // Matching - 100%
{
	struct ObjectAccess* oa;

	for (oa = objectAccess; oa->objectName != NULL; oa++)
	{
		if (oa->object == object)
		{
			oa->object = NULL;
			break;
		}
	}
}


struct Object* OBTABLE_FindObject(char* objectName) // Matching - 100%
{
	struct Object* object;
	struct _ObjectTracker* otr;
	int i;

	otr = gameTrackerX.GlobalObjects;

	for (i = 48; i != 0; i--, otr++)
	{
		if (otr->objectStatus != 0)
		{
			object = otr->object;

			if (MEMPACK_MemoryValidFunc((char*)object) != 0)
			{
				if (((unsigned int*)objectName)[0] == ((unsigned int*)object->name)[0])
				{
					if (((unsigned int*)objectName)[1] == ((unsigned int*)object->name)[1])
					{
						return object;
					}
				}
			}
		}
	}

	return 0;
}

void OBTABLE_ChangeObjectAccessPointers(struct Object* oldObject, struct Object* newObject) // Matching - 100%
{
	struct ObjectAccess* oa;

	for (oa = objectAccess; oa->objectName != NULL; oa++)
	{
		if (oa->object == oldObject)
		{
			oa->object = newObject;
			break;
		}
	}
}


void OBTABLE_RelocateObjectTune(struct Object* object, long offset) // Matching - 100%
{
	long id;

	id = object->id;

	if (id >= 0)
	{
		if (objectFunc[id].relocateTuneFunc != NULL)
		{
			objectFunc[id].relocateTuneFunc(object, offset);
		}
	}
	else
	{
		GenericRelocateTune(object, offset);
	}
}

void OBTABLE_RelocateInstanceObject(struct _Instance* instance, long offset) // Matching - 100%
{
	int id;

	id = instance->object->id;

	if (id >= 0)
	{
		if (objectFunc[id].relocateInstObFunc)
		{
			objectFunc[id].relocateInstObFunc(instance, offset);
		}
	}
}

void OBTABLE_InitAnimPointers(struct _ObjectTracker* objectTracker) // Matching - 100%
{
	struct Object* object;
	int i;
	char* earlyOut;
	struct _G2AnimKeylist_Type** keyPtr;
	struct _ObjectOwnerInfo* oi;
	struct _ObjectTracker* otr;
	struct Object* ownerOb;
	int j;
	int objectIndex;

	object = objectTracker->object;

	if ((object->oflags2 & 0x10000000))
	{
		keyPtr = object->animList;

		earlyOut = NULL;

		for (i = object->numAnims; i != 0; i--, keyPtr++)
		{
			oi = (struct _ObjectOwnerInfo*)keyPtr[0];

			if (oi->magicnum == 0xFACE0FF)
			{
				otr = STREAM_GetObjectTracker(oi->objectName);

				if (otr != NULL)
				{
					objectIndex = (objectTracker - gameTrackerX.GlobalObjects);

					ownerOb = otr->object;

					for (j = 0; j < otr->numObjectsUsing; j++)
					{
						if (otr->objectsUsing[j] == objectIndex)
						{
							break;
						}
					}

					if (j == otr->numObjectsUsing)
					{
						otr->numObjectsUsing += 1;
						otr->objectsUsing[j] = objectIndex;
					}

					if (otr->objectStatus == 2)
					{
						keyPtr[0] = ownerOb->animList[oi->animID];
					}
					else
					{
						earlyOut = oi->objectName;
					}
				}
			}
		}

		if (earlyOut != NULL)
		{
			return;
		}

		object->oflags2 &= 0xEFFFFFFF;
	}

	if ((object->oflags2 & 0x80000))
	{
		MonsterTranslateAnim(object);
	}
}