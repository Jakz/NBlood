//-------------------------------------------------------------------------
/*
Copyright (C) 2010-2019 EDuke32 developers and contributors
Copyright (C) 2019 Nuke.YKT

This file is part of NBlood.

NBlood is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compat.h"
#include "build.h"
#include "mmulti.h"
#include "common_game.h"
#include "actor.h"
#include "blood.h"
#include "db.h"
#include "callback.h"
#include "config.h"
#include "eventq.h"
#include "fx.h"
#include "gameutil.h"
#include "globals.h"
#include "levels.h"
#include "loadsave.h"
#include "player.h"
#include "qav.h"
#include "resource.h"
#include "seq.h"
#include "sfx.h"
#include "sound.h"
#include "trig.h"
#ifdef NOONE_EXTENSIONS
#include "nnexts.h"
#endif
#include "view.h"

#define kQAVEnd 125

void FirePitchfork(int, PLAYER *pPlayer);
void FireSpray(int, PLAYER *pPlayer);
void ThrowCan(int, PLAYER *pPlayer);
void DropCan(int, PLAYER *pPlayer);
void ExplodeCan(int, PLAYER *pPlayer);
void ThrowBundle(int, PLAYER *pPlayer);
void DropBundle(int, PLAYER *pPlayer);
void ExplodeBundle(int, PLAYER *pPlayer);
void ThrowProx(int, PLAYER *pPlayer);
void DropProx(int, PLAYER *pPlayer);
void ThrowRemote(int, PLAYER *pPlayer);
void DropRemote(int, PLAYER *pPlayer);
void FireRemote(int, PLAYER *pPlayer);
void FireShotgun(int nTrigger, PLAYER *pPlayer);
void EjectShell(int, PLAYER *pPlayer);
void FireTommy(int nTrigger, PLAYER *pPlayer);
void FireSpread(int nTrigger, PLAYER *pPlayer);
void AltFireSpread(int nTrigger, PLAYER *pPlayer);
void AltFireSpread2(int nTrigger, PLAYER *pPlayer);
void FireFlare(int nTrigger, PLAYER *pPlayer);
void AltFireFlare(int nTrigger, PLAYER *pPlayer);
void FireVoodoo(int nTrigger, PLAYER *pPlayer);
void AltFireVoodoo(int nTrigger, PLAYER *pPlayer);
void DropVoodoo(int nTrigger, PLAYER *pPlayer);
void FireTesla(int nTrigger, PLAYER *pPlayer);
void AltFireTesla(int nTrigger, PLAYER *pPlayer);
void FireNapalm(int nTrigger, PLAYER *pPlayer);
void FireNapalm2(int nTrigger, PLAYER *pPlayer);
void AltFireNapalm(int nTrigger, PLAYER *pPlayer);
void FireLifeLeech(int nTrigger, PLAYER *pPlayer);
void AltFireLifeLeech(int nTrigger, PLAYER *pPlayer);
void FireBeast(int nTrigger, PLAYER * pPlayer);

typedef void(*QAVTypeCast)(int, void *);

int nClientFirePitchfork = qavRegisterClient((QAVTypeCast)FirePitchfork);
int nClientFireSpray = qavRegisterClient((QAVTypeCast)FireSpray);
int nClientThrowCan = qavRegisterClient((QAVTypeCast)ThrowCan);
int nClientDropCan = qavRegisterClient((QAVTypeCast)DropCan);
int nClientExplodeCan = qavRegisterClient((QAVTypeCast)ExplodeCan);
int nClientThrowBundle = qavRegisterClient((QAVTypeCast)ThrowBundle);
int nClientDropBundle = qavRegisterClient((QAVTypeCast)DropBundle);
int nClientExplodeBundle = qavRegisterClient((QAVTypeCast)ExplodeBundle);
int nClientThrowProx = qavRegisterClient((QAVTypeCast)ThrowProx);
int nClientDropProx = qavRegisterClient((QAVTypeCast)DropProx);
int nClientThrowRemote = qavRegisterClient((QAVTypeCast)ThrowRemote);
int nClientDropRemote = qavRegisterClient((QAVTypeCast)DropRemote);
int nClientFireRemote = qavRegisterClient((QAVTypeCast)FireRemote);
int nClientFireShotgun = qavRegisterClient((QAVTypeCast)FireShotgun);
int nClientEjectShell = qavRegisterClient((QAVTypeCast)EjectShell);
int nClientFireTommy = qavRegisterClient((QAVTypeCast)FireTommy);
int nClientAltFireSpread2 = qavRegisterClient((QAVTypeCast)AltFireSpread2);
int nClientFireSpread = qavRegisterClient((QAVTypeCast)FireSpread);
int nClientAltFireSpread = qavRegisterClient((QAVTypeCast)AltFireSpread);
int nClientFireFlare = qavRegisterClient((QAVTypeCast)FireFlare);
int nClientAltFireFlare = qavRegisterClient((QAVTypeCast)AltFireFlare);
int nClientFireVoodoo = qavRegisterClient((QAVTypeCast)FireVoodoo);
int nClientAltFireVoodoo = qavRegisterClient((QAVTypeCast)AltFireVoodoo);
int nClientFireTesla = qavRegisterClient((QAVTypeCast)FireTesla);
int nClientAltFireTesla = qavRegisterClient((QAVTypeCast)AltFireTesla);
int nClientFireNapalm = qavRegisterClient((QAVTypeCast)FireNapalm);
int nClientFireNapalm2 = qavRegisterClient((QAVTypeCast)FireNapalm2);
int nClientFireLifeLeech = qavRegisterClient((QAVTypeCast)FireLifeLeech);
int nClientFireBeast = qavRegisterClient((QAVTypeCast)FireBeast);
int nClientAltFireLifeLeech = qavRegisterClient((QAVTypeCast)AltFireLifeLeech);
int nClientDropVoodoo = qavRegisterClient((QAVTypeCast)DropVoodoo);
int nClientAltFireNapalm = qavRegisterClient((QAVTypeCast)AltFireNapalm);

QAV *weaponQAV[kQAVEnd];

void QAV::PlaySound(int nSound)
{
    sndStartSample(nSound, -1, -1, 0);
}

void QAV::PlaySound3D(spritetype *pSprite, int nSound, int a3, int a4)
{
    sfxPlay3DSound(pSprite, nSound, a3, a4);
}

char sub_4B1A4(PLAYER *pPlayer)
{
    switch (pPlayer->curWeapon)
    {
    case 7:
        switch (pPlayer->weaponState)
        {
        case 5:
        case 6:
            return 1;
        }
        break;
    case 6:
        switch (pPlayer->weaponState)
        {
        case 4:
        case 5:
        case 6:
            return 1;
        }
        break;
    }
    return 0;
}

char BannedUnderwater(int nWeapon)
{
    return nWeapon == 7 || nWeapon == 6;
}

char sub_4B1FC(PLAYER *pPlayer, int a2, int a3, int a4)
{
    if (gInfiniteAmmo)
        return 1;
    if (a3 == -1)
        return 1;
    if (a2 == 12 && pPlayer->weaponAmmo == 11 && pPlayer->weaponState == 11)
        return 1;
    if (a2 == 9 && pPlayer->pXSprite->health > 0)
        return 1;
    return pPlayer->ammoCount[a3] >= a4;
}

char CheckAmmo(PLAYER *pPlayer, int a2, int a3)
{
    if (gInfiniteAmmo)
        return 1;
    if (a2 == -1)
        return 1;
    if (pPlayer->curWeapon == 12 && pPlayer->weaponAmmo == 11 && pPlayer->weaponState == 11)
        return 1;
    if (pPlayer->curWeapon == 9 && pPlayer->pXSprite->health >= (a3<<4))
        return 1;
    return pPlayer->ammoCount[a2] >= a3;
}

char sub_4B2C8(PLAYER *pPlayer, int a2, int a3)
{
    if (gInfiniteAmmo)
        return 1;
    if (a2 == -1)
        return 1;
    return pPlayer->ammoCount[a2] >= a3;
}

void SpawnBulletEject(PLAYER *pPlayer, int a2, int a3)
{
    POSTURE *pPosture = &pPlayer->pPosture[pPlayer->lifeMode][pPlayer->posture];
    pPlayer->zView = pPlayer->pSprite->z-pPosture->eyeAboveZ;
    int dz = pPlayer->zWeapon-(pPlayer->zWeapon-pPlayer->zView)/2;
    fxSpawnEjectingBrass(pPlayer->pSprite, dz, a2, a3);
}

void SpawnShellEject(PLAYER *pPlayer, int a2, int a3)
{
    POSTURE *pPosture = &pPlayer->pPosture[pPlayer->lifeMode][pPlayer->posture];
    pPlayer->zView = pPlayer->pSprite->z-pPosture->eyeAboveZ;
    int t = pPlayer->zWeapon - pPlayer->zView;
    int dz = pPlayer->zWeapon-t+(t>>2);
    fxSpawnEjectingShell(pPlayer->pSprite, dz, a2, a3);
}

void WeaponInit(void)
{
    for (int i = 0; i < kQAVEnd; i++)
    {
        DICTNODE *hRes = gSysRes.Lookup(i, "QAV");
        if (!hRes)
            ThrowError("Could not load QAV %d\n", i);
        weaponQAV[i] = (QAV*)gSysRes.Lock(hRes);
        weaponQAV[i]->nSprite = -1;
    }
}

void WeaponPrecache(void)
{
    for (int i = 0; i < kQAVEnd; i++)
    {
        if (weaponQAV[i])
            weaponQAV[i]->Precache();
    }
}

void WeaponDraw(PLAYER *pPlayer, int a2, int a3, int a4, int a5)
{
    dassert(pPlayer != NULL);
    if (pPlayer->weaponQav == -1)
        return;
    QAV * pQAV = weaponQAV[pPlayer->weaponQav];
    int v4;
    if (pPlayer->weaponTimer == 0)
        v4 = (int)totalclock % pQAV->at10;
    else
        v4 = pQAV->at10 - pPlayer->weaponTimer;
    pQAV->x = a3;
    pQAV->y = a4;
    int flags = 2;
    int nInv = powerupCheck(pPlayer, kPwUpShadowCloak);
    if (nInv >= 120 * 8 || (nInv != 0 && ((int)totalclock & 32)))
    {
        a2 = -128;
        flags |= 1;
    }
    pQAV->Draw(v4, flags, a2, a5);
}

void WeaponPlay(PLAYER *pPlayer)
{
    dassert(pPlayer != NULL);
    if (pPlayer->weaponQav == -1)
        return;
    QAV *pQAV = weaponQAV[pPlayer->weaponQav];
    pQAV->nSprite = pPlayer->pSprite->index;
    int nTicks = pQAV->at10 - pPlayer->weaponTimer;
    pQAV->Play(nTicks-4, nTicks, pPlayer->qavCallback, pPlayer);
}

void StartQAV(PLAYER *pPlayer, int nWeaponQAV, int a3 = -1, char a4 = 0)
{
    dassert(nWeaponQAV < kQAVEnd);
    pPlayer->weaponQav = nWeaponQAV;
    pPlayer->weaponTimer = weaponQAV[nWeaponQAV]->at10;
    pPlayer->qavCallback = a3;
    pPlayer->qavLoop = a4;
    weaponQAV[nWeaponQAV]->Preload();
    WeaponPlay(pPlayer);
    pPlayer->weaponTimer -= 4;
}

struct WEAPONTRACK
{
    int at0; // x aim speed
    int at4; // y aim speed
    int at8; // angle range
    int atc;
    int at10; // predict
    bool bIsProjectile;
};

WEAPONTRACK gWeaponTrack[] = {
    { 0, 0, 0, 0, 0, false },
    { 0x6000, 0x6000, 0x71, 0x55, 0x111111, false },
    { 0x8000, 0x8000, 0x71, 0x55, 0x2aaaaa, true },
    { 0x10000, 0x10000, 0x38, 0x1c, 0, false },
    { 0x6000, 0x8000, 0x38, 0x1c, 0, false },
    { 0x6000, 0x6000, 0x38, 0x1c, 0x2aaaaa, true },
    { 0x6000, 0x6000, 0x71, 0x55, 0, true },
    { 0x6000, 0x6000, 0x71, 0x38, 0, true },
    { 0x8000, 0x10000, 0x71, 0x55, 0x255555, true },
    { 0x10000, 0x10000, 0x71, 0, 0, true },
    { 0x10000, 0x10000, 0xaa, 0, 0, false },
    { 0x6000, 0x6000, 0x71, 0x55, 0, true },
    { 0x6000, 0x6000, 0x71, 0x55, 0, true },
    { 0x6000, 0x6000, 0x71, 0x55, 0, false },
};

void UpdateAimVector(PLAYER * pPlayer)
{
    short nSprite;
    spritetype *pSprite;
    dassert(pPlayer != NULL);
    spritetype *pPSprite = pPlayer->pSprite;
    int x = pPSprite->x;
    int y = pPSprite->y;
    int z = pPlayer->zWeapon;
    Aim aim;
    aim.dx = Cos(pPSprite->ang)>>16;
    aim.dy = Sin(pPSprite->ang)>>16;
    aim.dz = pPlayer->slope;
    WEAPONTRACK *pWeaponTrack = &gWeaponTrack[pPlayer->curWeapon];
    int nTarget = -1;
    pPlayer->aimTargetsCount = 0;
    if (gProfile[pPlayer->nPlayer].nAutoAim == 1 || (gProfile[pPlayer->nPlayer].nAutoAim == 2 && !pWeaponTrack->bIsProjectile) || pPlayer->curWeapon == 10 || pPlayer->curWeapon == 9)
    {
        int nClosest = 0x7fffffff;
        for (nSprite = headspritestat[kStatDude]; nSprite >= 0; nSprite = nextspritestat[nSprite])
        {
            pSprite = &sprite[nSprite];
            if (pSprite == pPSprite)
                continue;
            if (!gGameOptions.bFriendlyFire && IsTargetTeammate(pPlayer, pSprite))
                continue;
            if (pSprite->flags&32)
                continue;
            if (!(pSprite->flags&8))
                continue;
            int x2 = pSprite->x;
            int y2 = pSprite->y;
            int z2 = pSprite->z;
            int nDist = approxDist(x2-x, y2-y);
            if (nDist == 0 || nDist > 51200)
                continue;
            if (pWeaponTrack->at10)
            {
                int t = divscale(nDist,pWeaponTrack->at10, 12);
                x2 += (xvel[nSprite]*t)>>12;
                y2 += (yvel[nSprite]*t)>>12;
                z2 += (zvel[nSprite]*t)>>8;
            }
            int lx = x + mulscale30(Cos(pPSprite->ang), nDist);
            int ly = y + mulscale30(Sin(pPSprite->ang), nDist);
            int lz = z + mulscale(pPlayer->slope, nDist, 10);
            int zRange = mulscale(9460, nDist, 10);
            int top, bottom;
            GetSpriteExtents(pSprite, &top, &bottom);
            if (lz-zRange>bottom || lz+zRange<top)
                continue;
            int angle = getangle(x2-x,y2-y);
            if (klabs(((angle-pPSprite->ang+1024)&2047)-1024) > pWeaponTrack->at8)
                continue;
            if (pPlayer->aimTargetsCount < 16 && cansee(x,y,z,pPSprite->sectnum,x2,y2,z2,pSprite->sectnum))
                pPlayer->aimTargets[pPlayer->aimTargetsCount++] = nSprite;
            // Inlined?
            int dz = (lz-z2)>>8;
            int dy = (ly-y2)>>4;
            int dx = (lx-x2)>>4;
            int nDist2 = ksqrt(dx*dx+dy*dy+dz*dz);
            if (nDist2 >= nClosest)
                continue;
            DUDEINFO *pDudeInfo = getDudeInfo(pSprite->type);
            int center = (pSprite->yrepeat*pDudeInfo->aimHeight)<<2;
            int dzCenter = (z2-center)-z;
            if (cansee(x, y, z, pPSprite->sectnum, x2, y2, z2, pSprite->sectnum))
            {
                nClosest = nDist2;
                aim.dx = Cos(angle)>>16;
                aim.dy = Sin(angle)>>16;
                aim.dz = divscale(dzCenter, nDist, 10);
                nTarget = nSprite;
            }
        }
        if (pWeaponTrack->atc > 0)
        {
            for (nSprite = headspritestat[kStatThing]; nSprite >= 0; nSprite = nextspritestat[nSprite])
            {
                pSprite = &sprite[nSprite];
                if (!gGameOptions.bFriendlyFire && IsTargetTeammate(pPlayer, pSprite))
                    continue;
                if (!(pSprite->flags&8))
                    continue;
                int x2 = pSprite->x;
                int y2 = pSprite->y;
                int z2 = pSprite->z;
                int dx = x2-x;
                int dy = y2-y;
                int dz = z2-z;
                int nDist = approxDist(dx, dy);
                if (nDist == 0 || nDist > 51200)
                    continue;
                int lx = x + mulscale30(Cos(pPSprite->ang), nDist);
                int ly = y + mulscale30(Sin(pPSprite->ang), nDist);
                int lz = z + mulscale(pPlayer->slope, nDist, 10);
                int zRange = mulscale10(9460, nDist);
                int top, bottom;
                GetSpriteExtents(pSprite, &top, &bottom);
                if (lz-zRange>bottom || lz+zRange<top)
                    continue;
                int angle = getangle(dx,dy);
                if (klabs(((angle-pPSprite->ang+1024)&2047)-1024) > pWeaponTrack->atc)
                    continue;
                if (pPlayer->aimTargetsCount < 16 && cansee(x,y,z,pPSprite->sectnum,pSprite->x,pSprite->y,pSprite->z,pSprite->sectnum))
                    pPlayer->aimTargets[pPlayer->aimTargetsCount++] = nSprite;
                // Inlined?
                int dz2 = (lz-z2)>>8;
                int dy2 = (ly-y2)>>4;
                int dx2 = (lx-x2)>>4;
                int nDist2 = ksqrt(dx2*dx2+dy2*dy2+dz2*dz2);
                if (nDist2 >= nClosest)
                    continue;
                if (cansee(x, y, z, pPSprite->sectnum, pSprite->x, pSprite->y, pSprite->z, pSprite->sectnum))
                {
                    nClosest = nDist2;
                    aim.dx = Cos(angle)>>16;
                    aim.dy = Sin(angle)>>16;
                    aim.dz = divscale(dz, nDist, 10);
                    nTarget = nSprite;
                }
            }
        }
    }
    Aim aim2;
    aim2 = aim;
    RotateVector((int*)&aim2.dx, (int*)&aim2.dy, -pPSprite->ang);
    aim2.dz -= pPlayer->slope;
    pPlayer->relAim.dx = interpolate(pPlayer->relAim.dx, aim2.dx, pWeaponTrack->at0);
    pPlayer->relAim.dy = interpolate(pPlayer->relAim.dy, aim2.dy, pWeaponTrack->at0);
    pPlayer->relAim.dz = interpolate(pPlayer->relAim.dz, aim2.dz, pWeaponTrack->at4);
    pPlayer->aim = pPlayer->relAim;
    RotateVector((int*)&pPlayer->aim.dx, (int*)&pPlayer->aim.dy, pPSprite->ang);
    pPlayer->aim.dz += pPlayer->slope;
    pPlayer->aimTarget = nTarget;
}

struct t_WeaponModes
{
    int at0;
    int at4;
};

t_WeaponModes weaponModes[] = {
    { 0, -1 },
    { 1, -1 },
    { 1, 1 },
    { 1, 2 },
    { 1, 3 },
    { 1, 4 },
    { 1, 5 },
    { 1, 6 },
    { 1, 7 },
    { 1, 8 },
    { 1, 9 },
    { 1, 10 },
    { 1, 11 },
    { 0, -1 },
};

void WeaponRaise(PLAYER *pPlayer)
{
    dassert(pPlayer != NULL);
    int prevWeapon = pPlayer->curWeapon;
    pPlayer->curWeapon = pPlayer->input.newWeapon;
    pPlayer->input.newWeapon = 0;
    pPlayer->weaponAmmo = weaponModes[pPlayer->curWeapon].at4;
    switch (pPlayer->curWeapon)
    {
    case 1: // pitchfork
        pPlayer->weaponState = 0;
        StartQAV(pPlayer, 0, -1, 0);
        break;
    case 7: // spraycan
        if (pPlayer->weaponState == 2)
        {
            pPlayer->weaponState = 3;
            StartQAV(pPlayer, 8, -1, 0);
        }
        else
        {
            pPlayer->weaponState = 0;
            StartQAV(pPlayer, 4, -1, 0);
        }
        break;
    case 6: // dynamite
        if (gInfiniteAmmo || sub_4B2C8(pPlayer, 5, 1))
        {
            pPlayer->weaponState = 3;
            if (prevWeapon == 7)
                StartQAV(pPlayer, 16, -1, 0);
            else
                StartQAV(pPlayer, 18, -1, 0);
        }
        break;
    case 11: // proximity
        if (gInfiniteAmmo || sub_4B2C8(pPlayer, 10, 1))
        {
            pPlayer->weaponState = 7;
            StartQAV(pPlayer, 25, -1, 0);
        }
        break;
    case 12: // remote
        if (gInfiniteAmmo || sub_4B2C8(pPlayer, 11, 1))
        {
            pPlayer->weaponState = 10;
            StartQAV(pPlayer, 31, -1, 0);
        }
        else
        {
            StartQAV(pPlayer, 32, -1, 0);
            pPlayer->weaponState = 11;
        }
        break;
    case 3: // sawed off
        if (powerupCheck(pPlayer, kPwUpTwoGuns))
        {
            if (gInfiniteAmmo || pPlayer->ammoCount[2] >= 4)
                StartQAV(pPlayer, 59, -1, 0);
            else
                StartQAV(pPlayer, 50, -1, 0);
            if (gInfiniteAmmo || pPlayer->ammoCount[2] >= 4)
                pPlayer->weaponState = 7;
            else if (pPlayer->ammoCount[2] > 1)
                pPlayer->weaponState = 3;
            else if (pPlayer->ammoCount[2] > 0)
                pPlayer->weaponState = 2;
            else
                pPlayer->weaponState = 1;
        }
        else
        {
            if (gInfiniteAmmo || pPlayer->ammoCount[2] > 1)
                pPlayer->weaponState = 3;
            else if (pPlayer->ammoCount[2] > 0)
                pPlayer->weaponState = 2;
            else
                pPlayer->weaponState = 1;
            StartQAV(pPlayer, 50, -1, 0);
        }
        break;
    case 4: // tommy gun
        if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 3, 2))
        {
            pPlayer->weaponState = 1;
            StartQAV(pPlayer, 69, -1, 0);
        }
        else
        {
            pPlayer->weaponState = 0;
            StartQAV(pPlayer, 64, -1, 0);
        }
        break;
    case 10: // voodoo
        if (gInfiniteAmmo || sub_4B2C8(pPlayer, 9, 1))
        {
            pPlayer->weaponState = 2;
            StartQAV(pPlayer, 100, -1, 0);
        }
        break;
    case 2: // flaregun
        if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 1, 2))
        {
            StartQAV(pPlayer, 45, -1, 0);
            pPlayer->weaponState = 3;
        }
        else
        {
            StartQAV(pPlayer, 41, -1, 0);
            pPlayer->weaponState = 2;
        }
        break;
    case 8: // tesla cannon
        if (sub_4B2C8(pPlayer, 7, 1))
        {
            pPlayer->weaponState = 2;
            if (powerupCheck(pPlayer, kPwUpTwoGuns))
                StartQAV(pPlayer, 82, -1, 0);
            else
                StartQAV(pPlayer, 74, -1, 0);
        }
        else
        {
            pPlayer->weaponState = 3;
            StartQAV(pPlayer, 74, -1, 0);
        }
        break;
    case 5: // napalm
        if (powerupCheck(pPlayer, kPwUpTwoGuns))
        {
            StartQAV(pPlayer, 120, -1, 0);
            pPlayer->weaponState = 3;
        }
        else
        {
            StartQAV(pPlayer, 89, -1, 0);
            pPlayer->weaponState = 2;
        }
        break;
    case 9: // life leech
        pPlayer->weaponState = 2;
        StartQAV(pPlayer, 111, -1, 0);
        break;
    case 13: // beast
        pPlayer->weaponState = 2;
        StartQAV(pPlayer, 93, -1, 0);
        break;
    }
}

void WeaponLower(PLAYER *pPlayer)
{
    dassert(pPlayer != NULL);
    if (sub_4B1A4(pPlayer))
        return;
    pPlayer->throwPower = 0;
    int prevState = pPlayer->weaponState;
    switch (pPlayer->curWeapon)
    {
    case 1:
        StartQAV(pPlayer, 3, -1, 0);
        break;
    case 7:
        sfxKill3DSound(pPlayer->pSprite, -1, 441);
        switch (prevState)
        {
        case 1:
            StartQAV(pPlayer, 7, -1, 0);
            break;
        case 2:
            pPlayer->weaponState = 1;
            WeaponRaise(pPlayer);
            return;
        case 4:
            pPlayer->weaponState = 1;
            StartQAV(pPlayer, 11, -1, 0);
            pPlayer->input.newWeapon = 0;
            WeaponLower(pPlayer);
            break;
        case 3:
            if (pPlayer->input.newWeapon == 6)
            {
                pPlayer->weaponState = 2;
                StartQAV(pPlayer, 11, -1, 0);
                return;
            }
            else if (pPlayer->input.newWeapon == 7)
            {
                pPlayer->weaponState = 1;
                StartQAV(pPlayer, 11, -1, 0);
                pPlayer->input.newWeapon = 0;
                WeaponLower(pPlayer);
            }
            else
            {
                pPlayer->weaponState = 1;
                StartQAV(pPlayer, 11, -1, 0);
            }
            break;
        }
        break;
    case 6:
        switch (prevState)
        {
        case 1:
            StartQAV(pPlayer, 7, -1, 0);
            break;
        case 2:
            WeaponRaise(pPlayer);
            break;
        case 3:
            if (pPlayer->input.newWeapon == 7)
            {
                pPlayer->weaponState = 2;
                StartQAV(pPlayer, 17, -1, 0);
            }
            else
            {
                StartQAV(pPlayer, 19, -1, 0);
            }
            break;
        default:
            break;
        }
        break;
    case 11:
        switch (prevState)
        {
        case 7:
            StartQAV(pPlayer, 26, -1, 0);
            break;
        }
        break;
    case 12:
        switch (prevState)
        {
        case 10:
            StartQAV(pPlayer, 34, -1, 0);
            break;
        case 11:
            StartQAV(pPlayer, 35, -1, 0);
            break;
        }
        break;
    case 3:
        if (powerupCheck(pPlayer, kPwUpTwoGuns))
            StartQAV(pPlayer, 63, -1, 0);
        else
            StartQAV(pPlayer, 58, -1, 0);
        break;
    case 4:
        if (powerupCheck(pPlayer, kPwUpTwoGuns) && pPlayer->weaponState == 1)
            StartQAV(pPlayer, 72, -1, 0);
        else
            StartQAV(pPlayer, 68, -1, 0);
        break;
    case 2:
        if (powerupCheck(pPlayer, kPwUpTwoGuns) && pPlayer->weaponState == 3)
            StartQAV(pPlayer, 49, -1, 0);
        else
            StartQAV(pPlayer, 44, -1, 0);
        break;
    case 10:
        StartQAV(pPlayer, 109, -1, 0);
        break;
    case 8:
        if (sub_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, kPwUpTwoGuns))
            StartQAV(pPlayer, 88, -1, 0);
        else
            StartQAV(pPlayer, 81, -1, 0);
        break;
    case 5:
        if (powerupCheck(pPlayer, kPwUpTwoGuns))
            StartQAV(pPlayer, 124, -1, 0);
        else
            StartQAV(pPlayer, 92, -1, 0);
        break;
    case 9:
        StartQAV(pPlayer, 119, -1, 0);
        break;
    case 13:
        StartQAV(pPlayer, 99, -1, 0);
        break;
    }
    pPlayer->curWeapon = 0;
    pPlayer->qavLoop = 0;
}

void WeaponUpdateState(PLAYER *pPlayer)
{
    static int lastWeapon = 0;
    static int lastState = 0;
    XSPRITE *pXSprite = pPlayer->pXSprite;
    int va = pPlayer->curWeapon;
    int vb = pPlayer->weaponState;
    if (va != lastWeapon || vb != lastState)
    {
        lastWeapon = va;
        lastState = vb;
    }
    switch (lastWeapon)
    {
    case 1:
        pPlayer->weaponQav = 1;
        break;
    case 7:
        switch (vb)
        {
        case 0:
            pPlayer->weaponState = 1;
            StartQAV(pPlayer, 5, -1, 0);
            break;
        case 1:
            if (CheckAmmo(pPlayer, 6, 1))
            {
                pPlayer->weaponState = 3;
                StartQAV(pPlayer, 8, -1, 0);
            }
            else
                pPlayer->weaponQav = 6;
            break;
        case 3:
            pPlayer->weaponQav = 9;
            break;
        case 4:
            if (CheckAmmo(pPlayer, 6, 1))
            {
                pPlayer->weaponQav = 9;
                pPlayer->weaponState = 3;
            }
            else
            {
                pPlayer->weaponState = 1;
                StartQAV(pPlayer, 11, -1, 0);
            }
            sfxKill3DSound(pPlayer->pSprite, -1, 441);
            break;
        }
        break;
    case 6:
        switch (vb)
        {
        case 1:
            if (pPlayer->weaponAmmo == 5 && CheckAmmo(pPlayer, 5, 1))
            {
                pPlayer->weaponState = 3;
                StartQAV(pPlayer, 16, -1, 0);
            }
            break;
        case 0:
            pPlayer->weaponState = 1;
            StartQAV(pPlayer, 5, -1, 0);
            break;
        case 2:
            if (pPlayer->ammoCount[5] > 0)
            {
                pPlayer->weaponState = 3;
                StartQAV(pPlayer, 16, -1, 0);
            }
            else
                pPlayer->weaponQav = 6;
            break;
        case 3:
            pPlayer->weaponQav = 20;
            break;
        }
        break;
    case 11:
        switch (vb)
        {
        case 7:
            pPlayer->weaponQav = 27;
            break;
        case 8:
            pPlayer->weaponState = 7;
            StartQAV(pPlayer, 25, -1, 0);
            break;
        }
        break;
    case 12:
        switch (vb)
        {
        case 10:
            pPlayer->weaponQav = 36;
            break;
        case 11:
            pPlayer->weaponQav = 37;
            break;
        case 12:
            if (pPlayer->ammoCount[11] > 0)
            {
                pPlayer->weaponState = 10;
                StartQAV(pPlayer, 31, -1, 0);
            }
            else
                pPlayer->weaponState = -1;
            break;
        }
        break;
    case 3:
        switch (vb)
        {
        case 6:
            if (powerupCheck(pPlayer, kPwUpTwoGuns) && (gInfiniteAmmo || CheckAmmo(pPlayer, 2, 4)))
                pPlayer->weaponState = 7;
            else
                pPlayer->weaponState = 1;
            break;
        case 7:
            pPlayer->weaponQav = 60;
            break;
        case 1:
            if (CheckAmmo(pPlayer, 2, 1))
            {
                sfxPlay3DSound(pPlayer->pSprite, 410, 3, 2);
                StartQAV(pPlayer, 57, nClientEjectShell, 0);
                if (gInfiniteAmmo || pPlayer->ammoCount[2] > 1)
                    pPlayer->weaponState = 3;
                else
                    pPlayer->weaponState = 2;
            }
            else
                pPlayer->weaponQav = 51;
            break;
        case 2:
            pPlayer->weaponQav = 52;
            break;
        case 3:
            pPlayer->weaponQav = 53;
            break;
        }
        break;
    case 4:
        if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 3, 2))
        {
            pPlayer->weaponQav = 70;
            pPlayer->weaponState = 1;
        }
        else
        {
            pPlayer->weaponQav = 65;
            pPlayer->weaponState = 0;
        }
        break;
    case 2:
        if (powerupCheck(pPlayer, kPwUpTwoGuns))
        {
            if (vb == 3 && sub_4B2C8(pPlayer, 1, 2))
                pPlayer->weaponQav = 46;
            else
            {
                pPlayer->weaponQav = 42;
                pPlayer->weaponState = 2;
            }
        }
        else
            pPlayer->weaponQav = 42;
        break;
    case 10:
        if (pXSprite->height < 256 && klabs(pPlayer->swayHeight) > 768)
            pPlayer->weaponQav = 102;
        else
            pPlayer->weaponQav = 101;
        break;
    case 8:
        switch (vb)
        {
        case 2:
            if (sub_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, kPwUpTwoGuns))
                pPlayer->weaponQav = 83;
            else
                pPlayer->weaponQav = 75;
            break;
        case 3:
            pPlayer->weaponQav = 76;
            break;
        }
        break;
    case 5:
        switch (vb)
        {
        case 3:
            if (powerupCheck(pPlayer, kPwUpTwoGuns) && (gInfiniteAmmo || CheckAmmo(pPlayer,4, 4)))
                pPlayer->weaponQav = 121;
            else
                pPlayer->weaponQav = 90;
            break;
        case 2:
            pPlayer->weaponQav = 90;
            break;
        }
        break;
    case 9:
        switch (vb)
        {
        case 2:
            pPlayer->weaponQav = 112;
            break;
        }
        break;
    case 13:
        pPlayer->weaponQav = 94;
        break;
    }
}

void FirePitchfork(int, PLAYER *pPlayer)
{
    Aim *aim = &pPlayer->aim;
    int r1 = Random2(2000);
    int r2 = Random2(2000);
    int r3 = Random2(2000);
    for (int i = 0; i < 4; i++)
        actFireVector(pPlayer->pSprite, (2*i-3)*40, pPlayer->zWeapon-pPlayer->pSprite->z, aim->dx+r1, aim->dy+r2, aim->dz+r3, VECTOR_TYPE_0);
}

void FireSpray(int, PLAYER *pPlayer)
{
    playerFireMissile(pPlayer, 0, pPlayer->aim.dx, pPlayer->aim.dy, pPlayer->aim.dz, kMissileFlameSpray);
    UseAmmo(pPlayer, 6, 4);
    if (CheckAmmo(pPlayer, 6, 1))
        sfxPlay3DSound(pPlayer->pSprite, 441, 1, 2);
    else
        sfxKill3DSound(pPlayer->pSprite, -1, 441);
}

void ThrowCan(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, -1, 441);
    int nSpeed = mulscale16(pPlayer->throwPower, 0x177777)+0x66666;
    sfxPlay3DSound(pPlayer->pSprite, 455, 1, 0);
    spritetype *pSprite = playerFireThing(pPlayer, 0, -9460, kThingArmedSpray, nSpeed);
    if (pSprite)
    {
        sfxPlay3DSound(pSprite, 441, 0, 0);
        evPost(pSprite->index, 3, pPlayer->fuseTime, kCmdOn);
        int nXSprite = pSprite->extra;
        XSPRITE *pXSprite = &xsprite[nXSprite];
        pXSprite->Impact = 1;
        UseAmmo(pPlayer, 6, gAmmoItemData[0].count);
        pPlayer->throwPower = 0;
    }
}

void DropCan(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, -1, 441);
    spritetype *pSprite = playerFireThing(pPlayer, 0, 0, kThingArmedSpray, 0);
    if (pSprite)
    {
        evPost(pSprite->index, 3, pPlayer->fuseTime, kCmdOn);
        UseAmmo(pPlayer, 6, gAmmoItemData[0].count);
    }
}

void ExplodeCan(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, -1, 441);
    spritetype *pSprite = playerFireThing(pPlayer, 0, 0, kThingArmedSpray, 0);
    evPost(pSprite->index, 3, 0, kCmdOn);
    UseAmmo(pPlayer, 6, gAmmoItemData[0].count);
    StartQAV(pPlayer, 15, -1);
    pPlayer->curWeapon = 0;
    pPlayer->throwPower = 0;
}

void ThrowBundle(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, 16, -1);
    int nSpeed = mulscale16(pPlayer->throwPower, 0x177777)+0x66666;
    sfxPlay3DSound(pPlayer->pSprite, 455, 1, 0);
    spritetype *pSprite = playerFireThing(pPlayer, 0, -9460, kThingArmedTNTBundle, nSpeed);
    int nXSprite = pSprite->extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    if (pPlayer->fuseTime < 0)
        pXSprite->Impact = 1;
    else
        evPost(pSprite->index, 3, pPlayer->fuseTime, kCmdOn);
    UseAmmo(pPlayer, 5, 1);
    pPlayer->throwPower = 0;
}

void DropBundle(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, 16, -1);
    spritetype *pSprite = playerFireThing(pPlayer, 0, 0, kThingArmedTNTBundle, 0);
    evPost(pSprite->index, 3, pPlayer->fuseTime, kCmdOn);
    UseAmmo(pPlayer, 5, 1);
}

void ExplodeBundle(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, 16, -1);
    spritetype *pSprite = playerFireThing(pPlayer, 0, 0, kThingArmedTNTBundle, 0);
    evPost(pSprite->index, 3, 0, kCmdOn);
    UseAmmo(pPlayer, 5, 1);
    StartQAV(pPlayer, 24, -1, 0);
    pPlayer->curWeapon = 0;
    pPlayer->throwPower = 0;
}

void ThrowProx(int, PLAYER *pPlayer)
{
    int nSpeed = mulscale16(pPlayer->throwPower, 0x177777)+0x66666;
    sfxPlay3DSound(pPlayer->pSprite, 455, 1, 0);
    spritetype *pSprite = playerFireThing(pPlayer, 0, -9460, kThingArmedProxBomb, nSpeed);
    evPost(pSprite->index, 3, 240, kCmdOn);
    UseAmmo(pPlayer, 10, 1);
    pPlayer->throwPower = 0;
}

void DropProx(int, PLAYER *pPlayer)
{
    spritetype *pSprite = playerFireThing(pPlayer, 0, 0, kThingArmedProxBomb, 0);
    evPost(pSprite->index, 3, 240, kCmdOn);
    UseAmmo(pPlayer, 10, 1);
}

void ThrowRemote(int, PLAYER *pPlayer)
{
    int nSpeed = mulscale16(pPlayer->throwPower, 0x177777)+0x66666;
    sfxPlay3DSound(pPlayer->pSprite, 455, 1, 0);
    spritetype *pSprite = playerFireThing(pPlayer, 0, -9460, kThingArmedRemoteBomb, nSpeed);
    int nXSprite = pSprite->extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    pXSprite->rxID = 90+(pPlayer->pSprite->type-kDudePlayer1);
    UseAmmo(pPlayer, 11, 1);
    pPlayer->throwPower = 0;
}

void DropRemote(int, PLAYER *pPlayer)
{
    spritetype *pSprite = playerFireThing(pPlayer, 0, 0, kThingArmedRemoteBomb, 0);
    int nXSprite = pSprite->extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    pXSprite->rxID = 90+(pPlayer->pSprite->type-kDudePlayer1);
    UseAmmo(pPlayer, 11, 1);
}

void FireRemote(int, PLAYER *pPlayer)
{
    evSend(0, 0, 90+(pPlayer->pSprite->type-kDudePlayer1), kCmdOn);
}

#define kMaxShotgunBarrels 4

void FireShotgun(int nTrigger, PLAYER *pPlayer)
{
    dassert(nTrigger > 0 && nTrigger <= kMaxShotgunBarrels);
    if (nTrigger == 1)
    {
        sfxPlay3DSound(pPlayer->pSprite, 411, 2, 0);
        pPlayer->tiltEffect = 30;
        pPlayer->visibility = 20;
    }
    else
    {
        sfxPlay3DSound(pPlayer->pSprite, 412, 2, 0);
        pPlayer->tiltEffect = 50;
        pPlayer->visibility = 40;
    }
    int n = nTrigger<<4;
    for (int i = 0; i < n; i++)
    {
        int r1, r2, r3;
        VECTOR_TYPE nType;
        if (nTrigger == 1)
        {
            r1 = Random3(1500);
            r2 = Random3(1500);
            r3 = Random3(500);
            nType = VECTOR_TYPE_1;
        }
        else
        {
            r1 = Random3(2500);
            r2 = Random3(2500);
            r3 = Random3(1500);
            nType = VECTOR_TYPE_4;
        }
        actFireVector(pPlayer->pSprite, 0, pPlayer->zWeapon-pPlayer->pSprite->z, pPlayer->aim.dx+r1, pPlayer->aim.dy+r2, pPlayer->aim.dz+r3, nType);
    }
    UseAmmo(pPlayer, pPlayer->weaponAmmo, nTrigger);
    pPlayer->flashEffect = 1;
}

void EjectShell(int, PLAYER *pPlayer)
{
    SpawnShellEject(pPlayer, 25, 35);
    SpawnShellEject(pPlayer, 48, 35);
}

void FireTommy(int nTrigger, PLAYER *pPlayer)
{
    Aim *aim = &pPlayer->aim;
    sfxPlay3DSound(pPlayer->pSprite, 431, -1, 0);
    switch (nTrigger)
    {
    case 1:
    {
        int r1 = Random3(400);
        int r2 = Random3(1200);
        int r3 = Random3(1200);
        actFireVector(pPlayer->pSprite, 0, pPlayer->zWeapon-pPlayer->pSprite->z, aim->dx+r3, aim->dy+r2, aim->dz+r1, VECTOR_TYPE_5);
        SpawnBulletEject(pPlayer, -15, -45);
        pPlayer->visibility = 20;
        break;
    }
    case 2:
    {
        int r1 = Random3(400);
        int r2 = Random3(1200);
        int r3 = Random3(1200);
        actFireVector(pPlayer->pSprite, -120, pPlayer->zWeapon-pPlayer->pSprite->z, aim->dx+r3, aim->dy+r2, aim->dz+r1, VECTOR_TYPE_5);
        SpawnBulletEject(pPlayer, -140, -45);
        r1 = Random3(400);
        r2 = Random3(1200);
        r3 = Random3(1200);
        actFireVector(pPlayer->pSprite, 120, pPlayer->zWeapon-pPlayer->pSprite->z, aim->dx+r3, aim->dy+r2, aim->dz+r1, VECTOR_TYPE_5);
        SpawnBulletEject(pPlayer, 140, 45);
        pPlayer->visibility = 30;
        break;
    }
    }
    UseAmmo(pPlayer, pPlayer->weaponAmmo, nTrigger);
    pPlayer->flashEffect = 1;
}

#define kMaxSpread 14

void FireSpread(int nTrigger, PLAYER *pPlayer)
{
    dassert(nTrigger > 0 && nTrigger <= kMaxSpread);
    Aim *aim = &pPlayer->aim;
    int angle = (getangle(aim->dx, aim->dy)+((112*(nTrigger-1))/14-56))&2047;
    int dx = Cos(angle)>>16;
    int dy = Sin(angle)>>16;
    sfxPlay3DSound(pPlayer->pSprite, 431, -1, 0);
    int r1, r2, r3;
    r1 = Random3(300);
    r2 = Random3(600);
    r3 = Random3(600);
    actFireVector(pPlayer->pSprite, 0, pPlayer->zWeapon-pPlayer->pSprite->z, dx+r3, dy+r2, aim->dz+r1, VECTOR_TYPE_3);
    r1 = Random2(90);
    r2 = Random2(30);
    SpawnBulletEject(pPlayer, r2, r1);
    pPlayer->visibility = 20;
    UseAmmo(pPlayer, pPlayer->weaponAmmo, 1);
    pPlayer->flashEffect = 1;
}

void AltFireSpread(int nTrigger, PLAYER *pPlayer)
{
    dassert(nTrigger > 0 && nTrigger <= kMaxSpread);
    Aim *aim = &pPlayer->aim;
    int angle = (getangle(aim->dx, aim->dy)+((112*(nTrigger-1))/14-56))&2047;
    int dx = Cos(angle)>>16;
    int dy = Sin(angle)>>16;
    sfxPlay3DSound(pPlayer->pSprite, 431, -1, 0);
    int r1, r2, r3;
    r1 = Random3(300);
    r2 = Random3(600);
    r3 = Random3(600);
    actFireVector(pPlayer->pSprite, -120, pPlayer->zWeapon-pPlayer->pSprite->z, dx+r3, dy+r2, aim->dz+r1, VECTOR_TYPE_3);
    r1 = Random2(45);
    r2 = Random2(120);
    SpawnBulletEject(pPlayer, r2, r1);
    r1 = Random3(300);
    r2 = Random3(600);
    r3 = Random3(600);
    actFireVector(pPlayer->pSprite, 120, pPlayer->zWeapon-pPlayer->pSprite->z, dx+r3, dy+r2, aim->dz+r1, VECTOR_TYPE_3);
    r1 = Random2(-45);
    r2 = Random2(-120);
    SpawnBulletEject(pPlayer, r2, r1);
    pPlayer->tiltEffect = 20;
    pPlayer->visibility = 30;
    UseAmmo(pPlayer, pPlayer->weaponAmmo, 2);
    pPlayer->flashEffect = 1;
}

void AltFireSpread2(int nTrigger, PLAYER *pPlayer)
{
    dassert(nTrigger > 0 && nTrigger <= kMaxSpread);
    Aim *aim = &pPlayer->aim;
    int angle = (getangle(aim->dx, aim->dy)+((112*(nTrigger-1))/14-56))&2047;
    int dx = Cos(angle)>>16;
    int dy = Sin(angle)>>16;
    sfxPlay3DSound(pPlayer->pSprite, 431, -1, 0);
    if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 3, 2))
    {
        int r1, r2, r3;
        r1 = Random3(300);
        r2 = Random3(600);
        r3 = Random3(600);
        actFireVector(pPlayer->pSprite, -120, pPlayer->zWeapon-pPlayer->pSprite->z, dx+r3, dy+r2, aim->dz+r1, VECTOR_TYPE_3);
        r1 = Random2(45);
        r2 = Random2(120);
        SpawnBulletEject(pPlayer, r2, r1);
        r1 = Random3(300);
        r2 = Random3(600);
        r3 = Random3(600);
        actFireVector(pPlayer->pSprite, 120, pPlayer->zWeapon-pPlayer->pSprite->z, dx+r3, dy+r2, aim->dz+r1, VECTOR_TYPE_3);
        r1 = Random2(-45);
        r2 = Random2(-120);
        SpawnBulletEject(pPlayer, r2, r1);
        pPlayer->tiltEffect = 30;
        pPlayer->visibility = 45;
        UseAmmo(pPlayer, pPlayer->weaponAmmo, 2);
    }
    else
    {
        int r1, r2, r3;
        r1 = Random3(300);
        r2 = Random3(600);
        r3 = Random3(600);
        actFireVector(pPlayer->pSprite, 0, pPlayer->zWeapon-pPlayer->pSprite->z, dx+r3, dy+r2, aim->dz+r1, VECTOR_TYPE_3);
        r1 = Random2(90);
        r2 = Random2(30);
        SpawnBulletEject(pPlayer, r2, r1);
        pPlayer->tiltEffect = 20;
        pPlayer->visibility = 30;
        UseAmmo(pPlayer, pPlayer->weaponAmmo, 1);
    }
    pPlayer->flashEffect = 1;
    if (!sub_4B2C8(pPlayer, 3, 1))
    {
        WeaponLower(pPlayer);
        pPlayer->weaponState = -1;
    }
}

void FireFlare(int nTrigger, PLAYER *pPlayer)
{
    spritetype *pSprite = pPlayer->pSprite;
    int offset = 0;
    switch (nTrigger)
    {
    case 2:
        offset = -120;
        break;
    case 3:
        offset = 120;
        break;
    }
    playerFireMissile(pPlayer, offset, pPlayer->aim.dx, pPlayer->aim.dy, pPlayer->aim.dz, kMissileFlareRegular);
    UseAmmo(pPlayer, 1, 1);
    sfxPlay3DSound(pSprite, 420, 2, 0);
    pPlayer->visibility = 30;
    pPlayer->flashEffect = 1;
}

void AltFireFlare(int nTrigger, PLAYER *pPlayer)
{
    spritetype *pSprite = pPlayer->pSprite;
    int offset = 0;
    switch (nTrigger)
    {
    case 2:
        offset = -120;
        break;
    case 3:
        offset = 120;
        break;
    }
    playerFireMissile(pPlayer, offset, pPlayer->aim.dx, pPlayer->aim.dy, pPlayer->aim.dz, kMissileFlareAlt);
    UseAmmo(pPlayer, 1, 8);
    sfxPlay3DSound(pSprite, 420, 2, 0);
    pPlayer->visibility = 45;
    pPlayer->flashEffect = 1;
}

void FireVoodoo(int nTrigger, PLAYER *pPlayer)
{
    nTrigger--;
    int nSprite = pPlayer->nSprite;
    spritetype *pSprite = pPlayer->pSprite;
    if (nTrigger == 4)
    {
        actDamageSprite(nSprite, pSprite, DAMAGE_TYPE_2, 1<<4);
        return;
    }
    dassert(pPlayer->voodooTarget >= 0);
    spritetype *pTarget = &sprite[pPlayer->voodooTarget];
    if (!gGameOptions.bFriendlyFire && IsTargetTeammate(pPlayer, pTarget))
        return;
    switch (nTrigger)
    {
    case 0:
    {
        sfxPlay3DSound(pSprite, 460, 2, 0);
        fxSpawnBlood(pTarget, 17<<4);
        int nDamage = actDamageSprite(nSprite, pTarget, DAMAGE_TYPE_5, 17<<4);
        UseAmmo(pPlayer, 9, nDamage/4);
        break;
    }
    case 1:
    {
        sfxPlay3DSound(pSprite, 460, 2, 0);
        fxSpawnBlood(pTarget, 17<<4);
        int nDamage = actDamageSprite(nSprite, pTarget, DAMAGE_TYPE_5, 9<<4);
        if (IsPlayerSprite(pTarget))
            WeaponLower(&gPlayer[pTarget->type-kDudePlayer1]);
        UseAmmo(pPlayer, 9, nDamage/4);
        break;
    }
    case 3:
    {
        sfxPlay3DSound(pSprite, 463, 2, 0);
        fxSpawnBlood(pTarget, 17<<4);
        int nDamage = actDamageSprite(nSprite, pTarget, DAMAGE_TYPE_5, 49<<4);
        UseAmmo(pPlayer, 9, nDamage/4);
        break;
    }
    case 2:
    {
        sfxPlay3DSound(pSprite, 460, 2, 0);
        fxSpawnBlood(pTarget, 17<<4);
        int nDamage = actDamageSprite(nSprite, pTarget, DAMAGE_TYPE_5, 11<<4);
        if (IsPlayerSprite(pTarget))
        {
            PLAYER *pOtherPlayer = &gPlayer[pTarget->type - kDudePlayer1];
            pOtherPlayer->blindEffect = 128;
        }
        UseAmmo(pPlayer, 9, nDamage/4);
        break;
    }
    }
}

void AltFireVoodoo(int nTrigger, PLAYER *pPlayer)
{
    if (nTrigger == 2) {

        // by NoOne: trying to simulate v1.0x voodoo here.
        // dunno how exactly it works, but at least it not spend all the ammo on alt fire
        if (gGameOptions.weaponsV10x && !VanillaMode() && !DemoRecordStatus()) {
            int nCount = ClipHigh(pPlayer->ammoCount[9], pPlayer->aimTargetsCount);
            if (nCount > 0)
            {
                for (int i = 0; i < pPlayer->aimTargetsCount; i++)
                {
                    int nTarget = pPlayer->aimTargets[i];
                    spritetype* pTarget = &sprite[nTarget];
                    if (!gGameOptions.bFriendlyFire && IsTargetTeammate(pPlayer, pTarget))
                        continue;
                    int nDist = approxDist(pTarget->x - pPlayer->pSprite->x, pTarget->y - pPlayer->pSprite->y);
                    if (nDist > 0 && nDist < 51200)
                    {
                        int vc = pPlayer->ammoCount[9] >> 3;
                        int v8 = pPlayer->ammoCount[9] << 1;
                        int nDamage = (v8 + Random(vc)) << 4;
                        nDamage = (nDamage * ((51200 - nDist) + 1)) / 51200;
                        nDamage = actDamageSprite(pPlayer->nSprite, pTarget, DAMAGE_TYPE_5, nDamage);

                        if (IsPlayerSprite(pTarget))
                        {
                            PLAYER* pOtherPlayer = &gPlayer[pTarget->type - kDudePlayer1];
                            if (!pOtherPlayer->godMode || !powerupCheck(pOtherPlayer, kPwUpDeathMask))
                                powerupActivate(pOtherPlayer, kPwUpDeliriumShroom);
                        }
                        fxSpawnBlood(pTarget, 0);
                    }
                }
            }

            UseAmmo(pPlayer, 9, 20);
            pPlayer->weaponState = 0;
            return;
        }

        //int nAmmo = pPlayer->ammCount[9];
        int nCount = ClipHigh(pPlayer->ammoCount[9], pPlayer->aimTargetsCount);
        if (nCount > 0)
        {
            int v4 = pPlayer->ammoCount[9] - (pPlayer->ammoCount[9] / nCount) * nCount;
            for (int i = 0; i < pPlayer->aimTargetsCount; i++)
            {
                int nTarget = pPlayer->aimTargets[i];
                spritetype* pTarget = &sprite[nTarget];
                if (!gGameOptions.bFriendlyFire && IsTargetTeammate(pPlayer, pTarget))
                    continue;
                if (v4 > 0)
                    v4--;
                int nDist = approxDist(pTarget->x - pPlayer->pSprite->x, pTarget->y - pPlayer->pSprite->y);
                if (nDist > 0 && nDist < 51200)
                {
                    int vc = pPlayer->ammoCount[9] >> 3;
                    int v8 = pPlayer->ammoCount[9] << 1;
                    int nDamage = (v8 + Random2(vc)) << 4;
                    nDamage = (nDamage * ((51200 - nDist) + 1)) / 51200;
                    nDamage = actDamageSprite(pPlayer->nSprite, pTarget, DAMAGE_TYPE_5, nDamage);
                    UseAmmo(pPlayer, 9, nDamage);
                    if (IsPlayerSprite(pTarget))
                    {
                        PLAYER* pOtherPlayer = &gPlayer[pTarget->type - kDudePlayer1];
                        if (!pOtherPlayer->godMode || !powerupCheck(pOtherPlayer, kPwUpDeathMask))
                            powerupActivate(pOtherPlayer, kPwUpDeliriumShroom);
                    }
                    fxSpawnBlood(pTarget, 0);
                }
            }
        }
        UseAmmo(pPlayer, 9, pPlayer->ammoCount[9]);
        pPlayer->hasWeapon[10] = 0;
        pPlayer->weaponState = -1;
    }
}

void DropVoodoo(int nTrigger, PLAYER *pPlayer)
{
    UNREFERENCED_PARAMETER(nTrigger);
    sfxPlay3DSound(pPlayer->pSprite, 455, 2, 0);
    spritetype *pSprite = playerFireThing(pPlayer, 0, -4730, kThingVoodooHead, 0xccccc);
    if (pSprite)
    {
        int nXSprite = pSprite->extra;
        XSPRITE *pXSprite = &xsprite[nXSprite];
        pXSprite->data1 = pPlayer->ammoCount[9];
        evPost(pSprite->index, 3, 90, kCallbackDropVoodoo);
        UseAmmo(pPlayer, 6, gAmmoItemData[0].count);
        UseAmmo(pPlayer, 9, pPlayer->ammoCount[9]);
        pPlayer->hasWeapon[10] = 0;
    }
}

struct TeslaMissile
{
    int at0; // offset
    int at4; // id
    int at8; // ammo use
    int atc; // sound
    int at10; // light
    int at14; // weapon flash
};

void FireTesla(int nTrigger, PLAYER *pPlayer)
{
    TeslaMissile teslaMissile[6] = 
    {
        { 0, 306, 1, 470, 20, 1 },
        { -140, 306, 1, 470, 30, 1 },
        { 140, 306, 1, 470, 30, 1 },
        { 0, 302, 35, 471, 40, 1 },
        { -140, 302, 35, 471, 50, 1 },
        { 140, 302, 35, 471, 50, 1 },
    };
    if (nTrigger > 0 && nTrigger <= 6)
    {
        nTrigger--;
        spritetype *pSprite = pPlayer->pSprite;
        TeslaMissile *pMissile = &teslaMissile[nTrigger];
        if (!sub_4B2C8(pPlayer, 7, pMissile->at8))
        {
            pMissile = &teslaMissile[0];
            if (!sub_4B2C8(pPlayer, 7, pMissile->at8))
            {
                pPlayer->weaponState = -1;
                pPlayer->weaponQav = 76;
                pPlayer->flashEffect = 0;
                return;
            }
        }
        playerFireMissile(pPlayer, pMissile->at0, pPlayer->aim.dx, pPlayer->aim.dy, pPlayer->aim.dz, pMissile->at4);
        UseAmmo(pPlayer, 7, pMissile->at8);
        sfxPlay3DSound(pSprite, pMissile->atc, 1, 0);
        pPlayer->visibility = pMissile->at10;
        pPlayer->flashEffect = pMissile->at14;
    }
}

void AltFireTesla(int nTrigger, PLAYER *pPlayer)
{
    UNREFERENCED_PARAMETER(nTrigger);
    spritetype *pSprite = pPlayer->pSprite;
    playerFireMissile(pPlayer, 0, pPlayer->aim.dx, pPlayer->aim.dy, pPlayer->aim.dz, kMissileTeslaAlt);
    UseAmmo(pPlayer, pPlayer->weaponAmmo, 35);
    sfxPlay3DSound(pSprite, 471, 2, 0);
    pPlayer->visibility = 40;
    pPlayer->flashEffect = 1;
}

void FireNapalm(int nTrigger, PLAYER *pPlayer)
{
    spritetype *pSprite = pPlayer->pSprite;
    int offset = 0;
    switch (nTrigger)
    {
    case 2:
        offset = -50;
        break;
    case 3:
        offset = 50;
        break;
    }
    playerFireMissile(pPlayer, offset, pPlayer->aim.dx, pPlayer->aim.dy, pPlayer->aim.dz, kMissileFireballNapam);
    sfxPlay3DSound(pSprite, 480, 2, 0);
    UseAmmo(pPlayer, 4, 1);
    pPlayer->flashEffect = 1;
}

void FireNapalm2(int nTrigger, PLAYER *pPlayer)
{
    UNREFERENCED_PARAMETER(nTrigger);
    spritetype *pSprite = pPlayer->pSprite;
    playerFireMissile(pPlayer, -120, pPlayer->aim.dx, pPlayer->aim.dy, pPlayer->aim.dz, kMissileFireballNapam);
    playerFireMissile(pPlayer, 120, pPlayer->aim.dx, pPlayer->aim.dy, pPlayer->aim.dz, kMissileFireballNapam);
    sfxPlay3DSound(pSprite, 480, 2, 0);
    UseAmmo(pPlayer, 4, 2);
    pPlayer->flashEffect = 1;
}

void AltFireNapalm(int nTrigger, PLAYER *pPlayer)
{
    UNREFERENCED_PARAMETER(nTrigger);
    char UNUSED(bAkimbo) = powerupCheck(pPlayer, kPwUpTwoGuns);
    int nSpeed = mulscale16(0x8000, 0x177777)+0x66666;
    spritetype *pMissile = playerFireThing(pPlayer, 0, -4730, kThingNapalmBall, nSpeed);
    if (pMissile)
    {
        XSPRITE *pXSprite = &xsprite[pMissile->extra];
        pXSprite->data4 = ClipHigh(pPlayer->ammoCount[4], 12);
        UseAmmo(pPlayer, 4, pXSprite->data4);
        seqSpawn(22, 3, pMissile->extra, -1);
        actBurnSprite(actSpriteIdToOwnerId(pPlayer->pSprite->index), pXSprite, 600);
        evPost(pMissile->index, 3, 0, kCallbackFXFlameLick);
        sfxPlay3DSound(pMissile, 480, 2, 0);
        pPlayer->visibility = 30;
        pPlayer->flashEffect = 1;
    }
}

void FireLifeLeech(int nTrigger, PLAYER *pPlayer)
{
    if (!CheckAmmo(pPlayer, 8, 1))
        return;
    int r1 = Random2(2000);
    int r2 = Random2(2000);
    int r3 = Random2(1000);
    spritetype *pMissile = playerFireMissile(pPlayer, 0, pPlayer->aim.dx+r1, pPlayer->aim.dy+r2, pPlayer->aim.dz+r3, 315);
    if (pMissile)
    {
        XSPRITE *pXSprite = &xsprite[pMissile->extra];
        pXSprite->target = pPlayer->aimTarget;
        pMissile->ang = (nTrigger==2) ? 1024 : 0;
    }
    if (sub_4B2C8(pPlayer, 8, 1))
        UseAmmo(pPlayer, 8, 1);
    else
        actDamageSprite(pPlayer->nSprite, pPlayer->pSprite, DAMAGE_TYPE_5, 16);
    pPlayer->visibility = ClipHigh(pPlayer->visibility+5, 50);
}

void AltFireLifeLeech(int nTrigger, PLAYER *pPlayer)
{
    UNREFERENCED_PARAMETER(nTrigger);
    sfxPlay3DSound(pPlayer->pSprite, 455, 2, 0);
    spritetype *pMissile = playerFireThing(pPlayer, 0, -4730, kThingDroppedLifeLeech, 0x19999);
    if (pMissile)
    {
        pMissile->cstat |= 4096;
        XSPRITE *pXSprite = &xsprite[pMissile->extra];
        pXSprite->Push = 1;
        pXSprite->Proximity = 1;
        pXSprite->DudeLockout = 1;
        pXSprite->data4 = ClipHigh(pPlayer->ammoCount[4], 12);
        pXSprite->stateTimer = 1;
        evPost(pMissile->index, 3, 120, kCallbackLeechStateTimer);
        if (gGameOptions.nGameType <= 1)
        {
            int nAmmo = pPlayer->ammoCount[8];
            if (nAmmo < 25 && pPlayer->pXSprite->health > ((25-nAmmo)<<4))
            {
                actDamageSprite(pPlayer->nSprite, pPlayer->pSprite, DAMAGE_TYPE_5, ((25-nAmmo)<<4));
                nAmmo = 25;
            }
            pXSprite->data3 = nAmmo;
            UseAmmo(pPlayer, 8, nAmmo);
        }
        else
        {
            pXSprite->data3 = pPlayer->ammoCount[8];
            pPlayer->ammoCount[8] = 0;
        }
        pPlayer->hasWeapon[9] = 0;
    }
}

void FireBeast(int nTrigger, PLAYER * pPlayer)
{
    UNREFERENCED_PARAMETER(nTrigger);
    int r1 = Random2(2000);
    int r2 = Random2(2000);
    int r3 = Random2(2000);
    actFireVector(pPlayer->pSprite, 0, pPlayer->zWeapon-pPlayer->pSprite->z, pPlayer->aim.dx+r1, pPlayer->aim.dy+r2, pPlayer->aim.dz+r3, VECTOR_TYPE_9);
}

char gWeaponUpgrade[][13] = {
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
};

char WeaponUpgrade(PLAYER *pPlayer, char newWeapon)
{
    char weapon = pPlayer->curWeapon;
    if (!sub_4B1A4(pPlayer) && (gProfile[pPlayer->nPlayer].nWeaponSwitch&1) && (gWeaponUpgrade[pPlayer->curWeapon][newWeapon] || (gProfile[pPlayer->nPlayer].nWeaponSwitch&2)))
        weapon = newWeapon;
    return weapon;
}

int OrderNext[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 1 };
int OrderPrev[] = { 12, 12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1 };

char WeaponFindNext(PLAYER *pPlayer, int *a2, char bDir)
{
    int weapon = pPlayer->curWeapon;
    do
    {
        if (bDir)
            weapon = OrderNext[weapon];
        else
            weapon = OrderPrev[weapon];
        if (weaponModes[weapon].at0 && pPlayer->hasWeapon[weapon])
        {
            if (weapon == 9)
            {
                if (CheckAmmo(pPlayer, weaponModes[weapon].at4, 1))
                    break;
            }
            else
            {
                if (sub_4B2C8(pPlayer, weaponModes[weapon].at4, 1))
                    break;
            }
        }
    } while (weapon != pPlayer->curWeapon);
    if (weapon == pPlayer->curWeapon)
    {
        if (!weaponModes[weapon].at0 || !CheckAmmo(pPlayer, weaponModes[weapon].at4, 1))
            weapon = 1;
    }
    if (a2)
        *a2 = 0;
    return weapon;
}

char WeaponFindLoaded(PLAYER *pPlayer, int *a2)
{
    char v4 = 1;
    int v14 = 0;
    if (weaponModes[pPlayer->curWeapon].at0 > 1)
    {
        for (int i = 0; i < weaponModes[pPlayer->curWeapon].at0; i++)
        {
            if (CheckAmmo(pPlayer, weaponModes[pPlayer->curWeapon].at4, 1))
            {
                v14 = i;
                v4 = pPlayer->curWeapon;
                break;
            }
        }
    }
    if (v4 == 1)
    {
        int vc = 0;
        for (int i = 0; i < 14; i++)
        {
            int weapon = pPlayer->weaponOrder[vc][i];
            if (pPlayer->hasWeapon[weapon])
            {
                for (int j = 0; j < weaponModes[weapon].at0; j++)
                {
                    if (sub_4B1FC(pPlayer, weapon, weaponModes[weapon].at4, 1))
                    {
                        if (a2)
                            *a2 = j;
                        return weapon;
                    }
                }
            }
        }
    }
    else if (a2)
        *a2 = v14;
    return v4;
}

char sub_4F0E0(PLAYER *pPlayer)
{
    switch (pPlayer->weaponState)
    {
    case 5:
        if (!pPlayer->input.buttonFlags.shoot2)
            pPlayer->weaponState = 6;
        return 1;
    case 6:
        if (pPlayer->input.buttonFlags.shoot2)
        {
            pPlayer->weaponState = 3;
            pPlayer->fuseTime = pPlayer->weaponTimer;
            StartQAV(pPlayer, 13, nClientDropCan, 0);
        }
        else if (pPlayer->input.buttonFlags.shoot)
        {
            pPlayer->weaponState = 7;
            pPlayer->fuseTime = 0;
            pPlayer->throwTime = (int)gFrameClock;
        }
        return 1;
    case 7:
    {
        pPlayer->throwPower = ClipHigh(divscale16((int)gFrameClock-pPlayer->throwTime,240), 65536);
        if (!pPlayer->input.buttonFlags.shoot)
        {
            if (!pPlayer->fuseTime)
                pPlayer->fuseTime = pPlayer->weaponTimer;
            pPlayer->weaponState = 1;
            StartQAV(pPlayer, 14, nClientThrowCan, 0);
        }
        return 1;
    }
    }
    return 0;
}

char sub_4F200(PLAYER *pPlayer)
{
    switch (pPlayer->weaponState)
    {
    case 4:
        if (!pPlayer->input.buttonFlags.shoot2)
            pPlayer->weaponState = 5;
        return 1;
    case 5:
        if (pPlayer->input.buttonFlags.shoot2)
        {
            pPlayer->weaponState = 1;
            pPlayer->fuseTime = pPlayer->weaponTimer;
            StartQAV(pPlayer, 22, nClientDropBundle, 0);
        }
        else if (pPlayer->input.buttonFlags.shoot)
        {
            pPlayer->weaponState = 6;
            pPlayer->fuseTime = 0;
            pPlayer->throwTime = (int)gFrameClock;
        }
        return 1;
    case 6:
    {
        pPlayer->throwPower = ClipHigh(divscale16((int)gFrameClock-pPlayer->throwTime,240), 65536);
        if (!pPlayer->input.buttonFlags.shoot)
        {
            if (!pPlayer->fuseTime)
                pPlayer->fuseTime = pPlayer->weaponTimer;
            pPlayer->weaponState = 1;
            StartQAV(pPlayer, 23, nClientThrowBundle, 0);
        }
        return 1;
    }
    }
    return 0;
}

char sub_4F320(PLAYER *pPlayer)
{
    switch (pPlayer->weaponState)
    {
    case 9:
        pPlayer->throwPower = ClipHigh(divscale16((int)gFrameClock-pPlayer->throwTime,240), 65536);
        pPlayer->weaponTimer = 0;
        if (!pPlayer->input.buttonFlags.shoot)
        {
            pPlayer->weaponState = 8;
            StartQAV(pPlayer, 29, nClientThrowProx, 0);
        }
        break;
    }
    return 0;
}

char sub_4F3A0(PLAYER *pPlayer)
{
    switch (pPlayer->weaponState)
    {
    case 13:
        pPlayer->throwPower = ClipHigh(divscale16((int)gFrameClock-pPlayer->throwTime,240), 65536);
        if (!pPlayer->input.buttonFlags.shoot)
        {
            pPlayer->weaponState = 11;
            StartQAV(pPlayer, 39, nClientThrowRemote, 0);
        }
        break;
    }
    return 0;
}

char sub_4F414(PLAYER *pPlayer)
{
    switch (pPlayer->weaponState)
    {
    case 4:
        pPlayer->weaponState = 6;
        StartQAV(pPlayer, 114, nClientFireLifeLeech, 1);
        return 1;
    case 6:
        if (!pPlayer->input.buttonFlags.shoot2)
        {
            pPlayer->weaponState = 2;
            StartQAV(pPlayer, 118, -1, 0);
            return 1;
        }
        break;
    case 8:
        pPlayer->weaponState = 2;
        StartQAV(pPlayer, 118, -1, 0);
        return 1;
    }
    return 0;
}

char sub_4F484(PLAYER *pPlayer)
{
    switch (pPlayer->weaponState)
    {
    case 4:
        pPlayer->weaponState = 5;
        if (sub_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, kPwUpTwoGuns))
            StartQAV(pPlayer, 84, nClientFireTesla, 1);
        else
            StartQAV(pPlayer, 77, nClientFireTesla, 1);
        return 1;
    case 5:
        if (!pPlayer->input.buttonFlags.shoot)
        {
            pPlayer->weaponState = 2;
            if (sub_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, kPwUpTwoGuns))
                StartQAV(pPlayer, 87, -1, 0);
            else
                StartQAV(pPlayer, 80, -1, 0);
            return 1;
        }
        break;
    case 7:
        pPlayer->weaponState = 2;
        if (sub_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, kPwUpTwoGuns))
            StartQAV(pPlayer, 87, -1, 0);
        else
            StartQAV(pPlayer, 80, -1, 0);
        return 1;
    }
    return 0;
}

void WeaponProcess(PLAYER *pPlayer) {

    pPlayer->flashEffect = ClipLow(pPlayer->flashEffect - 1, 0);
    
    #ifdef NOONE_EXTENSIONS
    if (gPlayerCtrl[pPlayer->nPlayer].qavScene.index >= 0 && pPlayer->pXSprite->health > 0) {
        playerQavSceneProcess(pPlayer, &gPlayerCtrl[pPlayer->nPlayer].qavScene);
        UpdateAimVector(pPlayer);
        return;
    }
    #endif

    if (pPlayer->pXSprite->health == 0)
    {
        pPlayer->qavLoop = 0;
        sfxKill3DSound(pPlayer->pSprite, 1, -1);
    }
    if (pPlayer->isUnderwater && BannedUnderwater(pPlayer->curWeapon))
    {
        if (sub_4B1A4(pPlayer))
        {
            if (pPlayer->curWeapon == 7)
            {
                pPlayer->fuseTime = pPlayer->weaponTimer;
                DropCan(1, pPlayer);
                pPlayer->weaponState = 3;
            }
            else if (pPlayer->curWeapon == 6)
            {
                pPlayer->fuseTime = pPlayer->weaponTimer;
                DropBundle(1, pPlayer);
                pPlayer->weaponState = 1;
            }
        }
        WeaponLower(pPlayer);
        pPlayer->throwPower = 0;
    }
    WeaponPlay(pPlayer);
    UpdateAimVector(pPlayer);
    pPlayer->weaponTimer -= 4;
    char bShoot = pPlayer->input.buttonFlags.shoot;
    char bShoot2 = pPlayer->input.buttonFlags.shoot2;
    if (pPlayer->qavLoop && pPlayer->pXSprite->health > 0)
    {
        if (bShoot && CheckAmmo(pPlayer, pPlayer->weaponAmmo, 1))
        {
            while (pPlayer->weaponTimer <= 0)
                pPlayer->weaponTimer += weaponQAV[pPlayer->weaponQav]->at10;
        }
        else
        {
            pPlayer->weaponTimer = 0;
            pPlayer->qavLoop = 0;
        }
        return;
    }
    pPlayer->weaponTimer = ClipLow(pPlayer->weaponTimer, 0);
    switch (pPlayer->curWeapon)
    {
    case 7:
        if (sub_4F0E0(pPlayer))
            return;
        break;
    case 6:
        if (sub_4F200(pPlayer))
            return;
        break;
    case 11:
        if (sub_4F320(pPlayer))
            return;
        break;
    case 12:
        if (sub_4F3A0(pPlayer))
            return;
        break;
    }
    if (pPlayer->weaponTimer > 0)
        return;
    if (pPlayer->pXSprite->health == 0 || pPlayer->curWeapon == 0)
        pPlayer->weaponQav = -1;
    switch (pPlayer->curWeapon)
    {
    case 9:
        if (sub_4F414(pPlayer))
            return;
        break;
    case 8:
        if (sub_4F484(pPlayer))
            return;
        break;
    }
    if (pPlayer->nextWeapon)
    {
        sfxKill3DSound(pPlayer->pSprite, -1, 441);
        pPlayer->weaponState = 0;
        pPlayer->input.newWeapon = pPlayer->nextWeapon;
        pPlayer->nextWeapon = 0;
    }
    if (pPlayer->input.keyFlags.nextWeapon)
    {
        pPlayer->input.keyFlags.nextWeapon = 0;
        pPlayer->weaponState = 0;
        pPlayer->nextWeapon = 0;
        int t;
        char weapon = WeaponFindNext(pPlayer, &t, 1);
        pPlayer->weaponMode[weapon] = t;
        if (pPlayer->curWeapon)
        {
            WeaponLower(pPlayer);
            pPlayer->nextWeapon = weapon;
            return;
        }
        pPlayer->input.newWeapon = weapon;
    }
    if (pPlayer->input.keyFlags.prevWeapon)
    {
        pPlayer->input.keyFlags.prevWeapon = 0;
        pPlayer->weaponState = 0;
        pPlayer->nextWeapon = 0;
        int t;
        char weapon = WeaponFindNext(pPlayer, &t, 0);
        pPlayer->weaponMode[weapon] = t;
        if (pPlayer->curWeapon)
        {
            WeaponLower(pPlayer);
            pPlayer->nextWeapon = weapon;
            return;
        }
        pPlayer->input.newWeapon = weapon;
    }
    if (pPlayer->weaponState == -1)
    {
        pPlayer->weaponState = 0;
        int t;
        char weapon = WeaponFindLoaded(pPlayer, &t);
        pPlayer->weaponMode[weapon] = t;
        if (pPlayer->curWeapon)
        {
            WeaponLower(pPlayer);
            pPlayer->nextWeapon = weapon;
            return;
        }
        pPlayer->input.newWeapon = weapon;
    }
    if (pPlayer->input.newWeapon)
    {
        if (pPlayer->input.newWeapon == 6)
        {
            if (pPlayer->curWeapon == 6)
            {
                if (sub_4B2C8(pPlayer, 10, 1))
                    pPlayer->input.newWeapon = 11;
                else if (sub_4B2C8(pPlayer, 11, 1))
                    pPlayer->input.newWeapon = 12;
            }
            else if (pPlayer->curWeapon == 11)
            {
                if (sub_4B2C8(pPlayer, 11, 1))
                    pPlayer->input.newWeapon = 12;
                else if (sub_4B2C8(pPlayer, 5, 1) && pPlayer->isUnderwater == 0)
                    pPlayer->input.newWeapon = 6;
            }
            else if (pPlayer->curWeapon == 12)
            {
                if (sub_4B2C8(pPlayer, 5, 1) && pPlayer->isUnderwater == 0)
                    pPlayer->input.newWeapon = 6;
                else if (sub_4B2C8(pPlayer, 10, 1))
                    pPlayer->input.newWeapon = 11;
            }
            else
            {
                if (sub_4B2C8(pPlayer, 5, 1) && pPlayer->isUnderwater == 0)
                    pPlayer->input.newWeapon = 6;
                else if (sub_4B2C8(pPlayer, 10, 1))
                    pPlayer->input.newWeapon = 11;
                else if (sub_4B2C8(pPlayer, 11, 1))
                    pPlayer->input.newWeapon = 12;
            }
        }
        if (pPlayer->pXSprite->health == 0 || pPlayer->hasWeapon[pPlayer->input.newWeapon] == 0)
        {
            pPlayer->input.newWeapon = 0;
            return;
        }
        if (pPlayer->isUnderwater && BannedUnderwater(pPlayer->input.newWeapon) && !sub_4B1A4(pPlayer))
        {
            pPlayer->input.newWeapon = 0;
            return;
        }
        int nWeapon = pPlayer->input.newWeapon;
        int v4c = weaponModes[nWeapon].at0;
        if (!pPlayer->curWeapon)
        {
            int nAmmoType = weaponModes[nWeapon].at4;
            if (v4c > 1)
            {
                if (CheckAmmo(pPlayer, nAmmoType, 1) || nAmmoType == 11)
                    WeaponRaise(pPlayer);
                pPlayer->input.newWeapon = 0;
            }
            else
            {
                if (sub_4B1FC(pPlayer, nWeapon, nAmmoType, 1))
                    WeaponRaise(pPlayer);
                else
                {
                    pPlayer->weaponState = 0;
                    int t;
                    char weapon = WeaponFindLoaded(pPlayer, &t);
                    pPlayer->weaponMode[weapon] = t;
                    if (pPlayer->curWeapon)
                    {
                        WeaponLower(pPlayer);
                        pPlayer->nextWeapon = weapon;
                        return;
                    }
                    pPlayer->input.newWeapon = weapon;
                }
            }
            return;
        }
        if (nWeapon == pPlayer->curWeapon && v4c <= 1)
        {
            pPlayer->input.newWeapon = 0;
            return;
        }
        int i = 0;
        if (nWeapon == pPlayer->curWeapon)
            i = 1;
        for (; i <= v4c; i++)
        {
            int v6c = (pPlayer->weaponMode[nWeapon]+i)%v4c;
            if (sub_4B1FC(pPlayer, nWeapon, weaponModes[nWeapon].at4, 1))
            {
                WeaponLower(pPlayer);
                pPlayer->weaponMode[nWeapon] = v6c;
                return;
            }
        }
        pPlayer->input.newWeapon = 0;
        return;
    }
    if (pPlayer->curWeapon && !CheckAmmo(pPlayer, pPlayer->weaponAmmo, 1) && pPlayer->weaponAmmo != 11)
    {
        pPlayer->weaponState = -1;
        return;
    }
    if (bShoot)
    {
        switch (pPlayer->curWeapon)
        {
        case 1:
            StartQAV(pPlayer, 2, nClientFirePitchfork, 0);
            return;
        case 7:
            switch (pPlayer->weaponState)
            {
            case 3:
                pPlayer->weaponState = 4;
                StartQAV(pPlayer, 10, nClientFireSpray, 1);
                return;
            }
            break;
        case 6:
            switch (pPlayer->weaponState)
            {
            case 3:
                pPlayer->weaponState = 6;
                pPlayer->fuseTime = -1;
                pPlayer->throwTime = (int)gFrameClock;
                StartQAV(pPlayer, 21, nClientExplodeBundle, 0);
                return;
            }
            break;
        case 11:
            switch (pPlayer->weaponState)
            {
            case 7:
                pPlayer->weaponQav = 27;
                pPlayer->weaponState = 9;
                pPlayer->throwTime = (int)gFrameClock;
                return;
            }
            break;
        case 12:
            switch (pPlayer->weaponState)
            {
            case 10:
                pPlayer->weaponQav = 36;
                pPlayer->weaponState = 13;
                pPlayer->throwTime = (int)gFrameClock;
                return;
            case 11:
                pPlayer->weaponState = 12;
                StartQAV(pPlayer, 40, nClientFireRemote, 0);
                return;
            }
            break;
        case 3:
            switch (pPlayer->weaponState)
            {
            case 7:
                pPlayer->weaponState = 6;
                StartQAV(pPlayer, 61, nClientFireShotgun, 0);
                return;
            case 3:
                pPlayer->weaponState = 2;
                StartQAV(pPlayer, 54, nClientFireShotgun, 0);
                return;
            case 2:
                pPlayer->weaponState = 1;
                StartQAV(pPlayer, 55, nClientFireShotgun, 0);
                return;
            }
            break;
        case 4:
            if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 3, 2))
                StartQAV(pPlayer, 71, nClientFireTommy, 1);
            else
                StartQAV(pPlayer, 66, nClientFireTommy, 1);
            return;
        case 2:
            if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 1, 2))
                StartQAV(pPlayer, 48, nClientFireFlare, 0);
            else
                StartQAV(pPlayer, 43, nClientFireFlare, 0);
            return;
        case 10:
        {
            static int nChance[] = { 0xa000, 0xc000, 0xe000, 0x10000 };
            int nRand = wrand()*2;
            int i;
            for (i = 0; nChance[i] < nRand; i++)
            {
            }
            pPlayer->voodooTarget = pPlayer->aimTarget;
            if (pPlayer->voodooTarget == -1 || sprite[pPlayer->voodooTarget].statnum != kStatDude)
                i = 4;
            StartQAV(pPlayer,103+i, nClientFireVoodoo, 0);
            return;
        }
        case 8:
            switch (pPlayer->weaponState)
            {
            case 2:
                pPlayer->weaponState = 4;
                if (sub_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, kPwUpTwoGuns))
                    StartQAV(pPlayer, 84, nClientFireTesla, 0);
                else
                    StartQAV(pPlayer, 77, nClientFireTesla, 0);
                return;
            case 5:
                if (sub_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, kPwUpTwoGuns))
                    StartQAV(pPlayer, 84, nClientFireTesla, 0);
                else
                    StartQAV(pPlayer, 77, nClientFireTesla, 0);
                return;
            }
            break;
        case 5:
            if (powerupCheck(pPlayer, kPwUpTwoGuns))
                StartQAV(pPlayer, 122, nClientFireNapalm, 0);
            else
                StartQAV(pPlayer, 91, nClientFireNapalm, 0);
            return;
        case 9:
            sfxPlay3DSound(pPlayer->pSprite, 494, 2, 0);
            StartQAV(pPlayer, 116, nClientFireLifeLeech, 0);
            return;
        case 13:
            StartQAV(pPlayer, 95+Random(4), nClientFireBeast, 0);
            return;
        }
    }
    if (bShoot2)
    {
        switch (pPlayer->curWeapon)
        {
        case 1:
            StartQAV(pPlayer, 2, nClientFirePitchfork, 0);
            return;
        case 7:
            switch (pPlayer->weaponState)
            {
            case 3:
                pPlayer->weaponState = 5;
                StartQAV(pPlayer, 12, nClientExplodeCan, 0);
                return;
            }
            break;
        case 6:
            switch (pPlayer->weaponState)
            {
            case 3:
                pPlayer->weaponState = 4;
                StartQAV(pPlayer, 21, nClientExplodeBundle, 0);
                return;
            case 7:
                pPlayer->weaponState = 8;
                StartQAV(pPlayer, 28, nClientDropProx, 0);
                return;
            case 10:
                pPlayer->weaponState = 11;
                StartQAV(pPlayer, 38, nClientDropRemote, 0);
                return;
            case 11:
                if (pPlayer->ammoCount[11] > 0)
                {
                    pPlayer->weaponState = 10;
                    StartQAV(pPlayer, 30, -1, 0);
                }
                return;
            }
            break;
        case 11:
            switch (pPlayer->weaponState)
            {
            case 7:
                pPlayer->weaponState = 8;
                StartQAV(pPlayer, 28, nClientDropProx, 0);
                return;
            }
            break;
        case 12:
            switch (pPlayer->weaponState)
            {
            case 10:
                pPlayer->weaponState = 11;
                StartQAV(pPlayer, 38, nClientDropRemote, 0);
                return;
            case 11:
                if (pPlayer->ammoCount[11] > 0)
                {
                    pPlayer->weaponState = 10;
                    StartQAV(pPlayer, 30, -1, 0);
                }
                return;
            }
            break;
        case 3:
            switch (pPlayer->weaponState)
            {
            case 7:
                pPlayer->weaponState = 6;
                StartQAV(pPlayer, 62, nClientFireShotgun, 0);
                return;
            case 3:
                pPlayer->weaponState = 1;
                StartQAV(pPlayer, 56, nClientFireShotgun, 0);
                return;
            case 2:
                pPlayer->weaponState = 1;
                StartQAV(pPlayer, 55, nClientFireShotgun, 0);
                return;
            }
            break;
        case 4:
            if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 3, 2))
                StartQAV(pPlayer, 73, nClientAltFireSpread2, 0);
            else
                StartQAV(pPlayer, 67, nClientAltFireSpread2, 0);
            return;
        case 10:
            sfxPlay3DSound(pPlayer->pSprite, 461, 2, 0);
            StartQAV(pPlayer, 110, nClientAltFireVoodoo, 0);
            return;
#if 0
        case 2:
            if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 1, 2))
                StartQAV(pPlayer, 48, nClientFireFlare, 0);
            else
                StartQAV(pPlayer, 43, nClientFireFlare, 0);
            return;
#endif
        case 8:
            if (sub_4B2C8(pPlayer, 7, 35))
            {
                if (sub_4B2C8(pPlayer, 7, 70) && powerupCheck(pPlayer, kPwUpTwoGuns))
                    StartQAV(pPlayer, 85, nClientFireTesla, 0);
                else
                    StartQAV(pPlayer, 78, nClientFireTesla, 0);
            }
            else
            {
                if (sub_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, kPwUpTwoGuns))
                    StartQAV(pPlayer, 84, nClientFireTesla, 0);
                else
                    StartQAV(pPlayer, 77, nClientFireTesla, 0);
            }
            return;
        case 5:
            if (powerupCheck(pPlayer, kPwUpTwoGuns))
                // by NoOne: allow napalm launcher alt fire act like in v1.0x versions
                if (gGameOptions.weaponsV10x && !VanillaMode() && !DemoRecordStatus()) StartQAV(pPlayer, 123, nClientFireNapalm2, 0);
                else StartQAV(pPlayer, 122, nClientAltFireNapalm, 0);
            else
                StartQAV(pPlayer, 91, (gGameOptions.weaponsV10x && !VanillaMode() && !DemoRecordStatus()) ? nClientFireNapalm : nClientAltFireNapalm, 0);
            return;
        case 2:
            if (CheckAmmo(pPlayer, 1, 8))
            {
                if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 1, 16))
                    StartQAV(pPlayer, 48, nClientAltFireFlare, 0);
                else
                    StartQAV(pPlayer, 43, nClientAltFireFlare, 0);
            }
            else
            {
                if (powerupCheck(pPlayer, kPwUpTwoGuns) && sub_4B2C8(pPlayer, 1, 2))
                    StartQAV(pPlayer, 48, nClientFireFlare, 0);
                else
                    StartQAV(pPlayer, 43, nClientFireFlare, 0);
            }
            return;
        case 9:
            if (gGameOptions.nGameType <= 1 && !sub_4B2C8(pPlayer, 8, 1) && pPlayer->pXSprite->health < (25 << 4))
            {
                sfxPlay3DSound(pPlayer->pSprite, 494, 2, 0);
                StartQAV(pPlayer, 116, nClientFireLifeLeech, 0);
            }
            else
            {
                StartQAV(pPlayer, 119, -1, 0);
                AltFireLifeLeech(1, pPlayer);
                pPlayer->weaponState = -1;
            }
            return;
        }
    }
    WeaponUpdateState(pPlayer);
}

void sub_51340(spritetype *pMissile, int a2)
{
    char va4[(kMaxSectors+7)>>3];
    int x = pMissile->x;
    int y = pMissile->y;
    int z = pMissile->z;
    int nDist = 300;
    int nSector = pMissile->sectnum;
    int nOwner = actSpriteOwnerToSpriteId(pMissile);
    gAffectedSectors[0] = -1;
    gAffectedXWalls[0] = -1;
    GetClosestSpriteSectors(nSector, x, y, nDist, gAffectedSectors, va4, gAffectedXWalls);
    char v4 = 1;
    int v24 = -1;
    actHitcodeToData(a2, &gHitInfo, &v24, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (a2 == 3 && v24 >= 0 && sprite[v24].statnum == kStatDude)
        v4 = 0;
    for (int nSprite = headspritestat[kStatDude]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        if (nSprite != nOwner || v4)
        {
            spritetype *pSprite = &sprite[nSprite];
            if (pSprite->flags&32)
                continue;
            if (TestBitString(va4, pSprite->sectnum) && CheckProximity(pSprite, x, y, z, nSector, nDist))
            {
                int dx = pMissile->x-pSprite->x;
                int dy = pMissile->y-pSprite->y;
                int nDamage = ClipLow((nDist-(ksqrt(dx*dx+dy*dy)>>4)+20)>>1, 10);
                if (nSprite == nOwner)
                    nDamage /= 2;
                actDamageSprite(nOwner, pSprite, DAMAGE_TYPE_6, nDamage<<4);
            }
        }
    }
    for (int nSprite = headspritestat[kStatThing]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        spritetype *pSprite = &sprite[nSprite];
        if (pSprite->flags&32)
            continue;
        if (TestBitString(va4, pSprite->sectnum) && CheckProximity(pSprite, x, y, z, nSector, nDist))
        {
            XSPRITE *pXSprite = &xsprite[pSprite->extra];
            if (!pXSprite->locked)
            {
                int dx = pMissile->x-pSprite->x;
                int dy = pMissile->y-pSprite->y;
                int nDamage = ClipLow(nDist-(ksqrt(dx*dx+dy*dy)>>4)+20, 20);
                actDamageSprite(nOwner, pSprite, DAMAGE_TYPE_6, nDamage<<4);
            }
        }
    }
}

class WeaponLoadSave : public LoadSave
{
public:
    virtual void Load();
    virtual void Save();
};

void WeaponLoadSave::Load()
{
}

void WeaponLoadSave::Save()
{
}

static WeaponLoadSave *myLoadSave;

void WeaponLoadSaveConstruct(void)
{
    myLoadSave = new WeaponLoadSave();
}

