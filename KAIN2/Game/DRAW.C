#include "CORE.H"
#include "VRAM.H"
#include "DRAW.H"
#include "GAMELOOP.H"
#include "PSX/DRAWS.H"
#include "PSX/MAIN.H"
#include "LOCAL/LOCALSTR.H"
#include "FONT.H"
#include "MENU/MENU.H"
#include "PIPE3D.H"
#include "Game/PSX/COLLIDES.H"

short pause_redraw_flag; // offset 0x800d2fd4
void* pause_redraw_prim; // offset 0x800d2fd8
short RENDER_currentStreamUnitID;
SVECTOR shadow_vertices[11];
int InStreamUnit;
unsigned int** hackOT;

// autogenerated function stub: 
// void /*$ra*/ fDRAW_SPLIT_INTPL_XYZ(struct _SVector *newVertex /*$a0*/, struct _SVector *pvb /*$a1*/, struct _SVector *pvc /*$a2*/)
void fDRAW_SPLIT_INTPL_XYZ(struct _SVector *newVertex, struct _SVector *pvb, struct _SVector *pvc)
{ // line 61, offset 0x8002a5b0
	/* begin block 1 */
		// Start line: 122
	/* end block 1 */
	// End Line: 123

	/* begin block 2 */
		// Start line: 123
	/* end block 2 */
	// End Line: 124
	UNIMPLEMENTED();
}

void DRAW_InitShadow() // Matching - 100%
{
	int n;
	int inc;
	int deg;
	int ret;

	deg = 0;

	inc = 1677721;

	for (n = 0; n < 10; n++)
	{
		shadow_vertices[n].vx = rcos(deg / 4096) / 32;
		shadow_vertices[n].vy = rsin(deg / 4096) / 32;
		shadow_vertices[n].vz = 0;

		deg += inc;
	}

	shadow_vertices[10].vx = shadow_vertices[0].vx;
	shadow_vertices[10].vy = shadow_vertices[0].vy;
	shadow_vertices[10].vz = shadow_vertices[0].vz;
}

unsigned long* DRAW_DrawShadow(struct _PrimPool* primPool, struct _Model* model, unsigned long** ot, long fadeValue) // Matching - 97.17%
{
	struct POLY_G3_SEMITRANS* sg3;
	SVECTOR zero_vertex;
	long clip;
	long n;
	long p;
	long otz;
	unsigned long color;

	sg3 = (struct POLY_G3_SEMITRANS*)primPool->nextPrim;

	zero_vertex.vz = 0;
	zero_vertex.vy = 0;
	zero_vertex.vx = 0;
	gte_ldv0(&zero_vertex);

	// copFunction(2, 0x180001); // Perspective Transform
	gte_rtps(); // Perspective Transform.

	color = 0x00606060;

	gte_stdp(p);

	if (fadeValue > p && fadeValue < 0x1001)
	{
		p = fadeValue;
	}

	gte_ldrgb(color);
	gte_lddp(p);

	// copFunction(2, 0x0780010); // Depth Cueing.
	gte_dpcs(); // Depth Cueing.

	color = RGB2;
	color |= 0x32000000;

	for (n = 0; n < 10; n++)
	{
		gte_ldv1(&shadow_vertices[n]);
		gte_ldv2(&shadow_vertices[n + 1]);

		// copFunction(2, 0x280030); // Perspective transform on 3 points.
		gte_rtpt(); // Perspective transform on 3 points.

		if (primPool->lastPrim - 12 < (unsigned int*)sg3)
		{
			return (unsigned long*)sg3;
		}

		// copFunction(2, 0x1400006); // Normal clipping
		gte_nclip(); // Normal clipping
		gte_stopz(&clip);

		if (clip < 0)
		{
			// copFunction(2, 0x158002d); // Average of three Z values
			gte_avsz3(); // Average of three Z values
			gte_stotz(&otz);

			if (otz - (sizeof(struct POLY_G3_SEMITRANS) * 2) < (sizeof(struct POLY_G3_SEMITRANS) * 94))
			{
				gte_stsxy3(&sg3->x0, &sg3->x1, &sg3->x2);
				*(unsigned int*)&sg3->r0 = color;
				*(unsigned int*)&sg3->r1 = 0;
				*(unsigned int*)&sg3->r2 = 0;
				sg3->dr_tpage = 0xe1000640;
				sg3->tag = (((int)ot[otz] & 0x00FFFFFF) | 0x07000000);
				ot[otz] = (unsigned long*)((int)sg3 & 0x00FFFFFF);
				sg3++;
				primPool->numPrims++;
			}
		}
	}

	return (unsigned long*)sg3;
}

void DRAW_FlatQuad(CVECTOR *color, short x0, short y0, short x1, int y1, int x2, int y2, int x3, int y3, struct _PrimPool *primPool, unsigned long **ot)
{
	unsigned int *prim;

	prim = primPool->nextPrim;

	if ((char*)prim < (char*)primPool->lastPrim - sizeof(POLY_F4) * 2)
	{
		setXY4((POLY_F4*)prim, x0, y0, x1, y1, x2, y2, x3, y3);
		setRGB0((POLY_F4*)prim, color->r, color->g, color->b);
		setPolyF4((POLY_F4*)prim);
		addPrim(ot, prim);

		primPool->nextPrim = (unsigned int*)(((char*)primPool->nextPrim) + sizeof(POLY_F4));
		primPool->numPrims++;

	}
}

void DRAW_TranslucentQuad(short x0, short y0, short x1, short y1, int x2, int y2, int x3, int y3, int r, int g, int b, int abr, struct _PrimPool *primPool, unsigned long **ot)
{
	unsigned int *prim;

	prim = primPool->nextPrim;
	
	if (prim < primPool->lastPrim - 12)
	{
		setDrawTPage((POLY_F4_SEMITRANS*)prim, 0, 1, 8);
		setAbr((POLY_F4_SEMITRANS*)prim, abr);
		setPolyFT4_ST(prim);
		setRGB0((POLY_F4_SEMITRANS*)prim, r, g, b);
		setXY4((POLY_F4_SEMITRANS*)prim, x0, y0, x1, y1, x2, y2, x3, y3);
		addPrim(ot, prim);
		primPool->nextPrim += sizeof(POLY_F4_SEMITRANS) / sizeof(unsigned long);
		primPool->numPrims++;
	}
}

void DRAW_DrawButton(struct _ButtonTexture *button, short x, short y, unsigned long **ot)
{
	struct _PrimPool *primPool;
	unsigned int *prim;
	short w;
	short h;
	short offsetx;
	short offsety;

	primPool = gameTrackerX.primPool;

	prim = primPool->nextPrim;
	if (primPool->lastPrim - 12 >= primPool->nextPrim)
	{
		w = button->textureW << button->xshift;
		h = button->textureH;

		offsetx = (button->vramBlock->x & 0x3F) << button->xshift;
		offsety = button->vramBlock->y;

		SetPolyFT4((POLY_FT4*)prim);

		setShadeTex(((POLY_FT4*)prim), 1);

		((POLY_FT4*)prim)->tpage = button->tpage;

		((POLY_FT4*)prim)->v0 = (u_char)offsety;
		((POLY_FT4*)prim)->v1 = (u_char)offsety;

		((POLY_FT4*)prim)->x0 = x;
		((POLY_FT4*)prim)->y0 = y;

		((POLY_FT4*)prim)->u0 = (u_char)offsetx;

		((POLY_FT4*)prim)->x1 = (x + w) - 1;
		((POLY_FT4*)prim)->y1 = y;

		((POLY_FT4*)prim)->x1 = (x + w) - 1;
		((POLY_FT4*)prim)->y1 = y;

		((POLY_FT4*)prim)->u1 = (offsetx + w) - 1;

		((POLY_FT4*)prim)->x2 = x;
		((POLY_FT4*)prim)->y2 = (y + h) - 1;

		((POLY_FT4*)prim)->u2 = (u_char)offsetx;
		((POLY_FT4*)prim)->v2 = (offsety + h) - 1;

		((POLY_FT4*)prim)->x3 = (x + w) - 1;
		((POLY_FT4*)prim)->y3 = (y + h) - 1;

		((POLY_FT4*)prim)->u3 = (offsetx + w) - 1;
		((POLY_FT4*)prim)->v3 = (offsety + h) - 1;

		((POLY_FT4*)prim)->clut = button->clut;
		
		addPrim(ot, prim);

		primPool->nextPrim += sizeof(POLY_FT4) / sizeof(unsigned long);
		primPool->numPrims++;
	}
}

void DRAW_LoadButton(int *addr, struct _ButtonTexture *button)
{ 
	PSX_RECT vramRect;
	int *paletteAddr;
	short paletteW;
	short paletteH;

	paletteAddr = NULL;
	paletteW = 0;
	button->xshift = 0;
	paletteH = 0;

	if (((int*)addr)[1] == 8)
	{
		button->xshift = 2;
		paletteW = 16;
		paletteH = 1;
		paletteAddr = &addr[5];
		addr = &addr[13];
	}
	
	button->textureW = ((unsigned short*)addr)[4];
	button->textureH = ((unsigned short*)addr)[5];

	if (paletteW < button->textureW)
	{
		vramRect.w = button->textureW;
	}
	else
	{
		vramRect.w = paletteW;	
	}

	vramRect.h = button->textureH + paletteH;
	button->vramBlock = VRAM_CheckVramSlot(&vramRect.x, &vramRect.y, vramRect.w, vramRect.h, 4, 0);
	button->tpage = getTPage(2 - button->xshift, 0, vramRect.x, vramRect.y);
	button->vramBlock->udata.button = button;
	
	vramRect.w = button->textureW;
	vramRect.h = button->textureH;
	
	LoadImage(&vramRect, (unsigned int*)(addr + 3));
	
	if (paletteAddr != NULL)
	{
		vramRect.w = paletteW;
		vramRect.h = paletteH;
		vramRect.y += button->textureH;
		
		LoadImage(&vramRect, (unsigned int*)paletteAddr);

		button->clut = getClut(vramRect.x, vramRect.y);
	}

	DrawSync(0);
}

void DRAW_FreeButton(struct _ButtonTexture* button)
{
	VRAM_ClearVramBlock(button->vramBlock);
}


// autogenerated function stub: 
// void /*$ra*/ DRAW_RingLine(struct _PrimPool *primPool /*$t0*/, unsigned long **ot /*$t1*/, long color /*$a2*/)
void DRAW_RingLine(struct _PrimPool *primPool, unsigned long **ot, long color)
{ // line 3649, offset 0x8002ad6c
	/* begin block 1 */
		// Start line: 3650
		// Start offset: 0x8002AD6C
		// Variables:
			unsigned long *prim; // $a3
			long flag; // stack offset -24
			long clip; // stack offset -20
			long z0; // stack offset -16
			long z1; // stack offset -12
			long z2; // stack offset -8
			long otz; // $a0
			long p; // stack offset -4
	/* end block 1 */
	// End offset: 0x8002AE94
	// End Line: 3712

	/* begin block 2 */
		// Start line: 4652
	/* end block 2 */
	// End Line: 4653
			UNIMPLEMENTED();
}


// autogenerated function stub: 
// void /*$ra*/ DRAW_RingPoint(struct _PrimPool *primPool /*$a0*/, unsigned long **ot /*$a1*/, long color /*$s2*/, struct _SVector *vel /*$s1*/, struct _SVector *acc /*stack 16*/)
void DRAW_RingPoint(struct _PrimPool *primPool, unsigned long **ot, long color, struct _SVector *vel, struct _SVector *acc)
{ // line 3714, offset 0x8002ae9c
	/* begin block 1 */
		// Start line: 3715
		// Start offset: 0x8002AE9C
		// Variables:
			struct _SVector outpoint; // stack offset -24
	/* end block 1 */
	// End offset: 0x8002AF94
	// End Line: 3726

	/* begin block 2 */
		// Start line: 4789
	/* end block 2 */
	// End Line: 4790
			UNIMPLEMENTED();
}


// autogenerated function stub: 
// unsigned long * /*$ra*/ DRAW_DrawRingPoints(struct _Model *model /*$a0*/, struct _VertexPool *vertexPool /*$a1*/, MATRIX *pcTransform /*$s0*/, struct _PrimPool *primPool /*$s5*/, unsigned long **ot /*stack 16*/, long color /*stack 20*/, int ring_type /*stack 24*/)
unsigned long * DRAW_DrawRingPoints(struct _Model *model, struct _VertexPool *vertexPool, MATRIX *pcTransform, struct _PrimPool *primPool, unsigned long **ot, long color, int ring_type)
{ // line 3736, offset 0x8002afac
	/* begin block 1 */
		// Start line: 3737
		// Start offset: 0x8002AFAC
		// Variables:
			struct _MFace *mface; // $s1
			struct _MFace *endMFace; // $s4
			struct _PVertex *pvertexList; // $s3
			struct _SVector (*pvertex[3]); // stack offset -96
			struct _SVector newVertex[3]; // stack offset -80
			long outcode; // $a0
			int first; // $v1
			int second; // $a0
			struct _SVector vel; // stack offset -56
			struct _SVector acc; // stack offset -48
	/* end block 1 */
	// End offset: 0x8002B294
	// End Line: 3837

	/* begin block 2 */
		// Start line: 4841
	/* end block 2 */
	// End Line: 4842
			UNIMPLEMENTED();
	return null;
}

void DRAW_GlowQuad(struct _PrimPool* primPool, unsigned long** ot, long otz, long color, struct _Vector* v0, struct _Vector* v1, struct _Vector* v2, struct _Vector* v3)
{
	unsigned int* prim;
	struct _POLY_NG4* sg4;

	prim = primPool->nextPrim;

	sg4 = (struct _POLY_NG4*)prim;

	if (primPool->lastPrim - 12 >= prim)
	{
		primPool->numPrims++;

#if defined(PSXPC_VERSION)
		prim += 11;
#else
		prim += 10;
#endif

		if ((color & 0x1000000))
		{
			setDrawTPage(sg4, TRUE, TRUE, 64);

			color = color & 0xFFFFFF;
		}
		else
		{
			setDrawTPage(sg4, TRUE, TRUE, 32);
		}

		gte_lddp((short)(4096 - v0->z));
		gte_ldcv(&color);
		gte_gpf12();
		sg4->p1.x0 = (short)v0->x;
		sg4->p1.y0 = (short)v0->y;
		gte_stcv(&sg4->p1.r0);

		gte_lddp((short)(4096 - v1->z));
		gte_ldcv(&color);
		gte_gpf12();
		sg4->p1.x1 = (short)v1->x;
		sg4->p1.y1 = (short)v1->y;
		gte_stcv(&sg4->p1.r1);

		gte_lddp((short)(4096 - v2->z));
		gte_ldcv(&color);
		gte_gpf12();
		sg4->p1.x2 = (short)v2->x;
		sg4->p1.y2 = (short)v2->y;
		gte_stcv(&sg4->p1.r2);

		gte_lddp((short)(4096 - v3->z));
		gte_ldcv(&color);
		gte_gpf12();
		sg4->p1.x3 = (short)v3->x;
		sg4->p1.y3 = (short)v3->y;
		gte_stcv(&sg4->p1.r3);

		setlen(sg4, 9);

#if defined(PSXPC_VERSION)
		addPrim(ot[otz * 2], sg4);

#else
		addPrim(ot[otz], sg4);
#endif

		sg4->p1.code = 0x3A;

		primPool->nextPrim = prim;
	}
}

void DRAW_CreateAGlowingCircle(struct _Vector* f1, long z, struct _PrimPool* primPool, unsigned long** ot, long otz, long color, long w, long h, long angle)
{
	int v10; // $s1
	int v11; // $s3
	int v12; // $s0
	int v13; // $s3
	int v14; // $s0
	int v15; // $t5
	int x; // $v0
	int y; // $v1
	int v18; // $t3
	int v19; // $lo
	int v20; // $a0
	int v21; // $a0
	int v22; // $t2
	int v23; // $lo
	int v24; // $v0
	int v25; // $v1
	int v26; // $t0
	int v27; // $v1
	int v28; // $t0
	int v29; // $v1
	int v30; // $t0
	int v31; // $v1
	int v32; // $t0
	int v33; // $t8
	int v34; // $t9
	int v35; // $t2
	int v36; // $t1
	int v37; // $t2
	int v38; // $t1
	int v39; // $t2
	int v40; // $t1
	int v41; // $t2
	struct _Vector width; // [sp+24h] [-BCh]
	struct _Vector height; // [sp+30h] [-B0h] BYREF
	struct _Vector diag1; // [sp+40h] [-A0h]
	struct _Vector diag2; // [sp+50h] [-90h]
	_Vector points[9]; // [sp+60h] [-80h] BYREF
	struct _Vector* _v; // [sp+D4h] [-Ch]
	struct _Vector* _v1; // [sp+D8h] [-8h]

	f1->z = 0;
	if (z > 0 && otz < 3072)
	{
		v10 = 320 * w / z;
		height.z = 0;
		height.y = -(320 * h / z);
		rcos(angle);
		v11 = (rsin(angle) * height.y) >> 12;
		rsin(angle);
		height.x = (v11 << 9) / 240;
		height.y = (rcos(angle) * height.y) >> 12;
		v12 = rcos(angle);
		rsin(angle);
		v13 = (v12 * -v10) >> 12;
		v14 = rsin(angle);
		rcos(angle);
		v15 = (v13 << 9) / 240;
		width.x = (-v14 * -v10) >> 12;
		x = f1->x;
		y = f1->y;
		v18 = f1->z;
		_v1 = &height;
		diag1.x = x + height.x + v15;
		diag1.y = y + height.y + width.x;
		diag1.z = v18;
		v19 = 3072 * (diag1.x - f1->x);
		v20 = f1->y;
		diag1.z = v18 - f1->z;
		diag1.x = v19 >> 12;
		diag1.y = (3072 * (diag1.y - v20)) >> 12;
		v21 = f1->y + height.y - width.x;
		v22 = f1->z;
		diag2.x = f1->x + height.x - v15;
		diag2.y = v21;
		diag2.z = v22;
		v23 = 3072 * (diag2.x - f1->x);
		v24 = f1->y;
		diag2.z = v22 - f1->z;
		diag2.x = v23 >> 12;
		diag2.y = (3072 * (v21 - v24)) >> 12;
		v25 = f1->y + diag1.y;
		v26 = f1->z + diag1.z;
		points[0].x = f1->x + diag1.x;
		points[0].y = v25;
		points[0].z = v26;
		v27 = f1->y + height.y;
		v28 = f1->z;
		points[1].x = f1->x + height.x;
		points[1].y = v27;
		points[1].z = v28;
		v29 = f1->y + diag2.y;
		v30 = f1->z + diag2.z;
		points[2].x = f1->x + (v23 >> 12);
		_v = &points[2];
		points[2].y = v29;
		points[2].z = v30;
		v31 = f1->y + width.x;
		v32 = f1->z;
		points[3].x = f1->x + v15;
		points[3].y = v31;
		points[3].z = v32;
		v33 = f1->y;
		v34 = f1->z;
		points[4].x = f1->x;
		points[4].y = v33;
		points[4].z = v34;
		v35 = f1->y;
		v36 = f1->z;
		points[5].x = f1->x - v15;
		points[5].z = v36;
		points[5].y = v35 - width.x;
		v37 = f1->y;
		v38 = f1->z - diag2.z;
		points[6].x = f1->x - (v23 >> 12);
		points[6].z = v38;
		points[6].y = v37 - diag2.y;
		v39 = f1->y;
		v40 = f1->z;
		points[7].x = f1->x - height.x;
		points[7].y = v39 - height.y;
		points[7].z = v40;
		v41 = f1->y;
		points[8].x = f1->x - diag1.x;
		points[8].y = v41 - diag1.y;
		points[0].z = 4096;
		points[1].z = 4096;
		points[2].z = 4096;
		points[3].z = 4096;
		points[4].z = 0;
		points[5].z = 4096;
		points[6].z = 4096;
		points[7].z = 4096;
		points[8].z = 4096;
		DRAW_GlowQuad(primPool, ot, otz, color, &points[3], points, &points[4], &points[1]);
		DRAW_GlowQuad(primPool, ot, otz, color, &points[3], &points[4], &points[6], &points[7]);
		DRAW_GlowQuad(primPool, ot, otz, color, &points[1], _v, &points[4], &points[5]);
		DRAW_GlowQuad(primPool, ot, otz, color, &points[7], &points[4], &points[8], &points[5]);
	}
}

// autogenerated function stub: 
// unsigned long * /*$ra*/ DRAW_DrawGlowPoints2(struct _Instance *instance /*$s0*/, long seg1 /*$a1*/, long seg2 /*$s1*/, struct _PrimPool *primPool /*$s3*/, unsigned long **ot /*stack 16*/, long color /*stack 20*/, long height /*stack 24*/)
unsigned long * DRAW_DrawGlowPoints2(struct _Instance *instance, long seg1, long seg2, struct _PrimPool *primPool, unsigned long **ot, long color, long height)
{ // line 4038, offset 0x8002bad8
	/* begin block 1 */
		// Start line: 4039
		// Start offset: 0x8002BAD8
		// Variables:
			_Position glowPoints1; // stack offset -88
			_Position glowPoints2; // stack offset -80
			struct _Vector f1; // stack offset -72
			struct _Vector f2; // stack offset -56
			struct _Vector f3; // stack offset -40
			long otz; // $s2
			long otz2; // $s0
			long z; // $a1
			long length; // $s1
			long angle; // $v1
	/* end block 1 */
	// End offset: 0x8002BC24
	// End Line: 4079

	/* begin block 2 */
		// Start line: 5534
	/* end block 2 */
	// End Line: 5535
			UNIMPLEMENTED();
	return null;
}


// autogenerated function stub: 
// unsigned long * /*$ra*/ DRAW_DrawGlowPoint(struct _Instance *instance /*$a0*/, long seg1 /*$a1*/, struct _PrimPool *primPool /*$s3*/, unsigned long **ot /*$s4*/, long color /*stack 16*/, int width /*stack 20*/, int height /*stack 24*/)
unsigned long * DRAW_DrawGlowPoint(struct _Instance *instance, long seg1, struct _PrimPool *primPool, unsigned long **ot, long color, int width, int height)
{ // line 4089, offset 0x8002bc44
	/* begin block 1 */
		// Start line: 4090
		// Start offset: 0x8002BC44
		// Variables:
			_Position glowPoints1; // stack offset -48
			struct _Vector f1; // stack offset -40
			long otz; // $t0
			long z; // $a1
			long angle; // $s2
	/* end block 1 */
	// End offset: 0x8002BCF0
	// End Line: 4115

	/* begin block 2 */
		// Start line: 5682
	/* end block 2 */
	// End Line: 5683
			UNIMPLEMENTED();
	return null;
}

int DRAW_DisplayTFace_zclipped_C(SVECTOR* vertex0, SVECTOR* vertex1, SVECTOR* vertex2, struct UVTYPE* uv0, struct UVTYPE* uv1, struct UVTYPE* uv2, long color0, long color1, long color2, struct _PrimPool* primPool, unsigned int** ot, int ndiv)
{
	struct SP_SUBDIV_STRUCT* sp; // $t2
	int maxz; // $s3
	int point; // $a1
	int n; // $t7
	POLY_GT4* prim; // $t9
	long* ptr; // $t8
	long clip; // $s0
	int next_point_in_vv; // $v1
	int current_point_in_vv; // $t1
	int next; // $t6, $t0
	long clip_tmp; // $a0, $v1
	long zval; // stack offset -48
	SVECTOR* tmpptr; // $v0
	long zn; // $v1
	long znext; // $a0
	long interp1; // $t1
	long interp2; // $a2
	long x; // $v1
	long y; // $v1
	int flag; // $t3
	long opz; // stack offset -44
	long temp1; // $a3
	long temp2; // $t0
	long temp3; // $t1

	//fp = color0
	sp = (struct SP_SUBDIV_STRUCT*)getScratchAddr(128);
	//s4 = primPool

	//arg_0 = vertex0
	//arg_4 = vertex1
	//arg_8 = vertex2
	//arg_c = uv0

	gte_ldv0(vertex0);

	gte_rt();

	gte_stlvnl(&sp->out[0]);

	//s6 = vertex1

	gte_ldv0(vertex1);

	gte_rt();

	gte_stlvnl(&sp->out[1]);

	gte_ldv0(vertex2);

	gte_rt();

	gte_stlvnl(&sp->out[2]);

	//v0 = sp->out[0].z

	point = 0;
	//v0 = 2
	if (sp->out[0].z >= 160 || sp->out[1].z >= 160 || sp->out[2].z >= 160)
	{
		point = 0;

		//loc_8002C188
		maxz = 160;
		//s7 = uv0

		prim = (POLY_GT4*)primPool->nextPrim;

		
		sp->texinfo[0] = *(long*)uv0;

		clip = 0xFFFF;

		//s5 = uv1
		n = 0;

		sp->texinfo[1] = *(long*)uv1;

		//s2 = 0

		//s6 = uv2

		//a3 = sp

		//v1 = *(int*)uv2

		//s1 = sp

		sp->color[0] = color0;

		//v0 = color1

		ptr = (long*)&prim->y0;

		sp->color[1] = color1;

		//v0 = color2

		//t3 = &prim->y0

		sp->texinfo[2] = *(long*)uv2;
		sp->color[2] = color2;

		//loc_8002C1E4
		for(n = 0; n < 3; n++)
		{
			next = n + 1;

			if (next >= 3)
			{
				next = 0;
			}
			
			current_point_in_vv = (sp->out[n].z < 160) ^ 1;
			next_point_in_vv = (sp->out[next].z < 160) ^ 1;

			if (current_point_in_vv != 0)
			{
				if (n == 1)
				{
					tmpptr = vertex1;
				}
				else if (n < 2)
				{
					tmpptr = vertex0;
				}
				else if (n == 2)
				{
					tmpptr = vertex2;
				}
				else
				{
					tmpptr = vertex0;
				}

				gte_ldv0(tmpptr);
				gte_rtps();

				//v0 = sp->texinfo[0]

				((long*)&((short*)ptr)[1])[0] = sp->texinfo[n];

				//v0 = sp->color[0];
				((long*)&((short*)ptr)[-3])[0] = sp->color[n];

				tmpptr = (SVECTOR*)((short*)ptr - 1);

				gte_stsxy(tmpptr);

				gte_stsxy(&sp->sxy[point]);

				//v0 = &zval
				gte_stsz(&zval);

				if (maxz < zval)
				{
					maxz = zval;
				}

				//loc_8002C2C4

				x = ((short*)ptr)[-1];
				y = ((short*)ptr)[0];

				clip_tmp = (unsigned int)x >> 31;
				if (y < 0)
				{
					clip_tmp |= 0x2;
				}
				//loc_8002C2DC

				if (x >= SCREEN_WIDTH + 1)
				{
					clip_tmp |= 0x4;
				}

				if (y >= SCREEN_HEIGHT + 1)
				{
					clip_tmp |= 0x8;
				}

				clip &= clip_tmp;

				point++;//?
				ptr += 3;
			}
			//loc_8002C308
			if (current_point_in_vv != next_point_in_vv)
			{
				interp1 = ABS(((sp->out[n].z - 0xA0) << 12) / (sp->out[n].z - sp->out[next].z));

				//loc_8002C354
				zn = ((unsigned int)sp->out[next].x - (unsigned int)sp->out[n].x) * interp1;

				interp2 = 4096 - interp1;

				if (zn < 0)
				{
					zn += 4095;
				}

				//loc_8002C36C

				//v0 = ((sp->out[0].x) + (zn >> 12)) << 1
				zn = (((sp->out[n].x) + (zn >> 12)) << 1) + 256;

				if (zn >= 1024)
				{
					zn = 1023;
				}


				if (zn < -1023)
				{
					zn = -1023;
				}

				//loc_8002C39C
				((short*)ptr)[-1] = (short)zn;

				zn = (sp->out[next].y - sp->out[n].y) * interp1;//v0
				//v1 = sp->out[0].y
				if (zn < 0)
				{
					zn += 4095;
				}

				zn = ((sp->out[n].y + (zn >> 12)) << 1) + 120;//$v1

				if (zn >= 1024)
				{
					zn = 1023;
				}

				if (zn < -1023)
				{
					zn = -1023;
				}
				//loc_8002C3F4
				((short*)ptr)[0] = (short)zn;

				gte_lddp(interp2);

				IR1 = ((unsigned char*)&sp->texinfo[n])[0];//s2 probably i (before next)
				IR2 = ((unsigned char*)&sp->texinfo[n])[1];

				gte_gpf12();

				gte_lddp(interp1);
				IR1 = ((unsigned char*)&sp->texinfo[next])[0];
				IR2 = ((unsigned char*)&sp->texinfo[next])[1];

				gte_gpl12();

				//v0 = ptr + 8
				gte_stbv(&((char*)ptr)[2]);

				gte_lddp(interp2);


				IR1 = ((unsigned char*)&sp->color[n])[0];//s2 probably i (before next)
				IR2 = ((unsigned char*)&sp->color[n])[1];
				IR3 = ((unsigned char*)&sp->color[n])[2];

				gte_gpf12();

				gte_lddp(interp1);
				IR1 = ((unsigned char*)&sp->color[next])[0];
				IR2 = ((unsigned char*)&sp->color[next])[1];
				IR3 = ((unsigned char*)&sp->color[next])[2];

				gte_gpl12();

				gte_stcv(((short*)ptr - 3));

				//v0 = point << 2

				//v1 = (long*)(&((short*)ptr)[-1])[0];
				//v0 += sp
				*(long*)&sp->sxy[point] = *(long*)&((short*)ptr)[-1];

				x = ((short*)ptr)[-1];//v0
				y = ((short*)ptr)[0];//a0

				clip_tmp = (unsigned int)x >> 31;

				if (y < 0)
				{
					clip_tmp |= 0x2;
				}

				if (x >= SCREEN_WIDTH + 1)
				{
					clip_tmp |= 0x4;
				}

				if (y >= SCREEN_HEIGHT + 1)
				{
					clip_tmp |= 0x8;
				}

				clip &= clip_tmp;

				point++;//?
				ptr += 3;
			}
			//loc_8002C52C
		}

		//v0 = 2
		if (clip == 0)
		{
			maxz >>= 2;

			if (maxz < 3072)
			{
				if (ndiv == 0)
				{
					flag = 0;

					if (point > 0)
					{
						//loc_8002C57C
						for (n = 0; n < point; n++)
						{
							next = n + 1;

							if (next >= point)
							{
								next = 0;
							}

							temp1 = sp->sxy[n].vx - sp->sxy[next].vx;

							if (temp1 < 0)
							{
								temp1 = sp->sxy[next].vx - sp->sxy[n].vx;
							}

							if (temp1 >= 1024)
							{
								flag = 1;
								break;
							}
							else
							{
								temp1 = sp->sxy[n].vy - sp->sxy[next].vy;

								if (temp1 < 0)
								{
									temp1 = sp->sxy[next].vy - sp->sxy[n].vy;

									if (temp1 >= 512)
									{
										flag = 1;
										break;
									}
								}
								else
								{
									if (temp1 >= 512)
									{
										flag = 1;
										break;
									}
								}
							}
						}
					}

					//loc_8002C614
					if (flag == 0)
					{
						if (point == 4)
						{
							temp2 = sp->sxy[3].vx - sp->sxy[1].vx;

							if (temp2 < 0)
							{
								temp2 = sp->sxy[1].vx - sp->sxy[3].vx;
							}

							if (temp2 < 1024)
							{
								temp3 = sp->sxy[1].vy - sp->sxy[3].vy;

								if (temp3 < 0)
								{
									temp3 = sp->sxy[3].vy - sp->sxy[1].vy;

									if (temp3 >= 512)
									{
										flag = 1;
									}
								}
								else if (temp3 >= 512)
								{
									flag = 1;
								}
							}
							else
							{
								//loc_8002C6A0
								flag = 1;
							}
						}
						//loc_8002C6A4
						if (flag != 0)
						{
							gte_ldv0(vertex0);
							gte_ldv1(vertex1);
							gte_ldv2(vertex2);

							((CVECTOR*)&sp->color[1])->cd = ((CVECTOR*)&sp->color[0])->cd;

							gte_ldrgb3(&sp->color[0], &sp->color[1], &sp->color[2]);

							gteRegs.CP2D.p[6].sd = sp->color[2];//hack

							*(long*)&prim->u0 = *(long*)&uv0->u;

							*(long*)&prim->u1 = *(long*)&uv1->u;

							*(long*)&prim->u2 = *(long*)&uv2->u;

							DRAW_Zclip_subdiv((POLY_GT3*)prim, ot, 1);

							return 1;
						}
					}
					else
					{

						//loc_8002C6AC
						//s6 = vertex0
						//s7 = vertex1
						//s5 = vertex2

						gte_ldv0(vertex0);
						gte_ldv1(vertex1);
						gte_ldv2(vertex2);

						//a0 = &sp->color[0];
						//v0 = ((CVECTOR)&sp->color[0])->cd

						//v1 = &sp->color[1]
						//v0 = ((CVECTOR)&sp->color[1])->cd
						((CVECTOR*)&sp->color[1])->cd = ((CVECTOR*)&sp->color[0])->cd;
						//v0 = &sp->color[2];

						gte_ldrgb3(&sp->color[0], &sp->color[1], &sp->color[2]);

						gteRegs.CP2D.p[6].sd = sp->color[2];//hack

						//s6 = uv0
						*(long*)&prim->u0 = *(long*)&uv0->u;
						*(long*)&prim->u1 = *(long*)&uv1->u;
						*(long*)&prim->u2 = *(long*)&uv2->u;

						DRAW_Zclip_subdiv((POLY_GT3*)prim, ot, 1);

						return 1;
					}
				}
				//loc_8002C744
				SXY0 = *(long*)&prim->x0;
				SXY1 = *(long*)&prim->x1;
				SXY2 = *(long*)&prim->x2;

				gte_nclip();

				prim->clut = ((unsigned short*)&sp->texinfo)[1];
				prim->tpage = ((unsigned short*)&sp->texinfo[1])[1];

				gte_stopz(&opz);

				if (opz > 0)
				{
					return 4;
				}

				if (point == 3)
				{
					//a2 = 0xFFFFFF
					//v0 = color0 >> 24
					//a1 = maxz << 2

					prim->code = color0 >> 24;

					//s6 = ot

					//v0 = 1

					setlen(prim, 9);

#if defined(PSXPC_VERSION)
					addPrim(ot[maxz * 2], prim);
#else
					addPrim(ot[maxz], prim);
#endif
					primPool->nextPrim += sizeof(POLY_GT3) / sizeof(unsigned long);
				
					primPool->numPrims++;

					return 1;
				}
				//loc_8002C7F0
				if (point == 4)
				{
					//a2 = 0xFFFFFF
					//v0 = color0 >> 24

					temp1 = *(long*)&prim->r2;//t0
					temp2 = *(long*)&prim->x2;//t1
					temp3 = *(long*)&prim->u2;//t2

					//v1 = *(long*)&prim->r3;
					//a0 = *(long*)&prim->x3;
					//a1 = *(long*)&prim->u3;

					prim->code = (color0 >> 24) | 0x8;

					//v0 = 1

					*(long*)&prim->r2 = *(long*)&prim->r3;
					*(long*)&prim->x2 = *(long*)&prim->x3;
					*(long*)&prim->u2 = *(long*)&prim->u3;

					*(long*)&prim->r3 = temp1;
					*(long*)&prim->x3 = temp2;
					*(long*)&prim->u3 = temp3;

					//s7 = ot
					//a1 = ot[maxz]


					setlen(prim, 12);

#if defined(PSXPC_VERSION)
					addPrim(ot[maxz * 2], prim);
#else
					addPrim(ot[maxz], prim);
#endif
					primPool->nextPrim += sizeof(POLY_GT4) / sizeof(unsigned long);

					primPool->numPrims++;

					return 1;
				}
			}
			//loc_8002C7F8
		}
		//loc_8002C884
	}
	//loc_8002C884
	return 2;
}

int* DRAW_Zclip_subdiv(POLY_GT3* texture, unsigned int** ot, int ndiv)
{
	struct SP_SUBDIV_STRUCT* sp;
	struct _PrimPool* primPool;
	int clip;

	sp = (struct SP_SUBDIV_STRUCT*)getScratchAddr(128);

	if (ndiv != 0)
	{
		sp = (struct SP_SUBDIV_STRUCT*)getScratchAddr(181);
	}
	
	gte_strgb3(&sp->color0, &sp->color1, &sp->color2);
	
	((int*)&sp->vertex0)[0] = ((int*)&VX0)[0];
	((int*)&sp->vertex0)[1] = ((int*)&VZ0)[0];

	((int*)&sp->vertex1)[0] = ((int*)&VX1)[0];
	((int*)&sp->vertex1)[1] = ((int*)&VZ1)[0];

	((int*)&sp->vertex2)[0] = ((int*)&VX2)[0];
	((int*)&sp->vertex2)[1] = ((int*)&VZ2)[0];

	primPool = gameTrackerX.primPool;

	primPool->nextPrim = (unsigned int*)texture;
	
	((int*)&sp->face_v1.vx)[0] = ((int*)&sp->vertex1.x)[0];
	((int*)&sp->face_v0.vz)[0] = ((int*)&sp->vertex0.z)[0];
	((int*)&sp->face_v1.vz)[0] = ((int*)&sp->vertex1.z)[0];
	((int*)&sp->face_v2.vx)[0] = ((int*)&sp->vertex2.x)[0];
	((int*)&sp->face_v2.vz)[0] = ((int*)&sp->vertex2.z)[0];
	((int*)&sp->face_v0.vx)[0] = ((int*)&sp->vertex0.x)[0];
	
	sp->face_v01.vx = (((((unsigned int)sp->face_v0.vx + (unsigned int)sp->face_v1.vx) + (((unsigned int)sp->face_v0.vx + (unsigned int)sp->face_v1.vx))) >> 31) >> 1);
	sp->face_v01.vy = (((((unsigned int)sp->face_v0.vy + (unsigned int)sp->face_v1.vy) + (((unsigned int)sp->face_v0.vy + (unsigned int)sp->face_v1.vy))) >> 31) >> 1);
	sp->face_v01.vz = (((((unsigned int)sp->face_v0.vz + (unsigned int)sp->face_v1.vz) + (((unsigned int)sp->face_v0.vz + (unsigned int)sp->face_v1.vz))) >> 31) >> 1);

	sp->face_v12.vx = (((((unsigned int)sp->face_v1.vx + (unsigned int)sp->face_v2.vx) + (((unsigned int)sp->face_v1.vx + (unsigned int)sp->face_v2.vx))) >> 31) >> 1);
	sp->face_v12.vy = (((((unsigned int)sp->face_v1.vy + (unsigned int)sp->face_v2.vy) + (((unsigned int)sp->face_v1.vy + (unsigned int)sp->face_v2.vy))) >> 31) >> 1);
	sp->face_v12.vz = (((((unsigned int)sp->face_v1.vz + (unsigned int)sp->face_v2.vz) + (((unsigned int)sp->face_v1.vz + (unsigned int)sp->face_v2.vz))) >> 31) >> 1);

	sp->face_v20.vx = (((((unsigned int)sp->face_v2.vx + (unsigned int)sp->face_v0.vx) + (((unsigned int)sp->face_v2.vx + (unsigned int)sp->face_v0.vx))) >> 31) >> 1);
	sp->face_v20.vy = (((((unsigned int)sp->face_v2.vy + (unsigned int)sp->face_v0.vy) + (((unsigned int)sp->face_v2.vy + (unsigned int)sp->face_v0.vy))) >> 31) >> 1);
	sp->face_v20.vz = (((((unsigned int)sp->face_v2.vz + (unsigned int)sp->face_v0.vz) + (((unsigned int)sp->face_v2.vz + (unsigned int)sp->face_v0.vz))) >> 31) >> 1);

	((int*)&sp->face_uv0)[0] = ((int*)texture)[4];
	((int*)&sp->face_uv1)[0] = ((int*)texture)[7];
	((int*)&sp->face_uv2)[0] = ((int*)texture)[10];
	
	sp->face_uv01.u = (sp->face_uv0.u + sp->face_uv1.u) >> 1;
	sp->face_uv12.pad = sp->face_uv1.pad;
	sp->face_uv01.pad = sp->face_uv1.pad;
	sp->face_uv01.v = (sp->face_uv0.v + sp->face_uv1.v) >> 1;
	sp->face_uv12.u = (sp->face_uv1.u + sp->face_uv2.u) >> 1;
	sp->face_uv12.v = (sp->face_uv1.v + sp->face_uv2.v) >> 1;
	sp->face_uv20.u = (sp->face_uv2.u + sp->face_uv0.u) >> 1;
	sp->face_uv20.v = (sp->face_uv2.v + sp->face_uv0.v) >> 1;
	sp->face_uv20.pad = sp->face_uv0.pad;
	
	sp->color01.r = (sp->color0.r + sp->color1.r) >> 1;
	sp->color01.g = (sp->color0.g + sp->color1.g) >> 1;
	sp->color01.b = (sp->color0.b + sp->color1.b) >> 1;
	
	sp->color12.r = (sp->color1.r + sp->color2.r) >> 1;
	sp->color12.g = (sp->color1.g + sp->color2.g) >> 1;
	sp->color12.b = (sp->color1.b + sp->color2.b) >> 1;
	
	sp->color20.r = (sp->color2.r + sp->color0.r) >> 1;
	sp->color20.g = (sp->color2.g + sp->color0.g) >> 1;
	sp->color20.b = (sp->color2.b + sp->color0.b) >> 1;

	sp->color20.code = sp->color0.code;
	sp->color01.code = sp->color0.code;

	clip = DRAW_DisplayTFace_zclipped_C(&sp->face_v0, &sp->face_v01, &sp->face_v20, &sp->face_uv0, &sp->face_uv01, &sp->face_uv20, ((int*)&sp->color0)[0], ((int*)&sp->color01)[0], ((int*)&sp->color20)[0], primPool, ot, ndiv);
	clip |= DRAW_DisplayTFace_zclipped_C(&sp->face_v20, &sp->face_v12, &sp->face_v2, &sp->face_uv20, &sp->face_uv12, &sp->face_uv2, ((int*)&sp->color20)[0], ((int*)&sp->color12)[0], ((int*)&sp->color2)[0], primPool, ot, ndiv);
	
	sp->face_uv01.pad = sp->face_uv0.pad;
	sp->face_uv20.pad = sp->face_uv1.pad;
	
	clip |= DRAW_DisplayTFace_zclipped_C(&sp->face_v01, &sp->face_v12, &sp->face_v20, &sp->face_uv01, &sp->face_uv12, &sp->face_uv20, ((int*)&sp->color01)[0], ((int*)&sp->color12)[0], ((int*)&sp->color20)[0], primPool, ot, ndiv);
	clip |= DRAW_DisplayTFace_zclipped_C(&sp->face_v01, &sp->face_v1, &sp->face_v12, &sp->face_uv01, &sp->face_uv1, &sp->face_uv12, ((int*)&sp->color01)[0], ((int*)&sp->color1)[0], ((int*)&sp->color12)[0], primPool, ot, ndiv);
	
	if ((clip & 0x5) != 0x4)
	{
		return (int*)primPool->nextPrim;
	}

	return NULL;
}

void DRAW_LoadingMessage()
{
	unsigned long **drawot;

#if !defined(PSXPC_VERSION)
	while (CheckVolatile(gameTrackerX.drawTimerReturn) != 0)
	{

	}

	while (CheckVolatile(gameTrackerX.reqDisp) != 0)
	{

	}
#endif

	DrawSyncCallback(NULL);
	VSyncCallback(NULL);

	PutDrawEnv(&draw[gameTrackerX.drawPage ^ 1]);
	
	FONT_FontPrintCentered(localstr_get(LOCALSTR_Hint52), 150);
	
	DisplayHintBox(FONT_GetStringWidth(localstr_get(LOCALSTR_Hint52)), 150);
	
	FONT_Flush();

	drawot = gameTrackerX.drawOT;

#if defined(PSX_VERSION)
#if defined(USE_32_BIT_ADDR)
	DrawOTag((unsigned int*)drawot + 3071 * 2);
#else
	DrawOTag((unsigned int*)drawot + 3071);
#endif
#endif

	DrawSync(0);
	ClearOTagR((unsigned int*)drawot, 3072);
	PutDrawEnv(&draw[gameTrackerX.drawPage]);
	VSyncCallback(VblTick);
	DrawSyncCallback(DrawCallback);
}




