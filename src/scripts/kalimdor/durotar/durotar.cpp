/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2010 MaNGOSZero <http://github.com/mangoszero/mangoszero/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "scriptPCH.h"

enum LazyPeon
{
    QUEST_LAZY_PEONS  = 5441,
    GO_LUMBERPILE     = 175784,
    SPELL_BUFF_SLEEP  = 17743,
    SPELL_AWAKEN_PEON = 19938,
    EMOTE_WORKING     = 234,
    SAY_HIT           = 5774, // TODO: there are probably more random texts
    SAY_STOP          = 5056
};

enum States
{
    STATE_SLEEPING             = 0,
    STATE_MOVING_TO_LUMBERPILE = 1,
    STATE_WORKING              = 2,
    STATE_MOVING_BACK          = 3,
};

struct LazyPeonAI : public ScriptedAI
{
    explicit LazyPeonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiTimeBeforeSleep{};
    uint8 m_uiState{};
    ObjectGuid m_uiPlayerGuid{};

    void Reset() override
    {
        m_uiTimeBeforeSleep = 20000; // work for 20 seconds
        m_uiState = STATE_SLEEPING;
        m_uiPlayerGuid.Clear();
    }

    void SpellHit(SpellCaster* caster, SpellEntry const* spell) override
    {

        if (spell->Id != SPELL_AWAKEN_PEON)
        {
            return;
        }

        Player* player = caster->ToPlayer();
        if (!player)
        {
            return;
        }

        if (!m_creature->HasAura(SPELL_BUFF_SLEEP))
        {
            DoScriptText(SAY_STOP, m_creature, player);
            return;
        }

        if (player->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
        {
            player->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetGUID());
            DoScriptText(SAY_HIT, m_creature, player);
            m_creature->RemoveAurasDueToSpell(SPELL_BUFF_SLEEP);

            // Start moving
            if (GameObject* pLumberPile = m_creature->FindNearestGameObject(GO_LUMBERPILE, 20.0f))
            {
                m_creature->SetWalk(true);
                float fX, fY, fZ;
                pLumberPile->GetContactPoint(m_creature, fX, fY, fZ, CONTACT_DISTANCE + 0.2f);
                m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ, MOVE_PATHFINDING);
                m_uiState = STATE_MOVING_TO_LUMBERPILE;
            }
        }
    }

    void UpdateAI(uint32 const diff) override
    {
        switch (m_uiState)
        {
            case STATE_WORKING:
            {
                if (m_uiTimeBeforeSleep < diff)
                {
                    m_uiState = STATE_MOVING_BACK;
                    m_creature->HandleEmoteState(0);
                    float x, y, z, o;
                    m_creature->GetHomePosition(x, y, z, o);
                    m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
                }
                else
                {
                    m_uiTimeBeforeSleep -= diff;
                }
                break;
            }
            case STATE_SLEEPING:
            {
                if (!m_creature->HasAura(SPELL_BUFF_SLEEP))
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BUFF_SLEEP) == CAST_OK)
                    {
                        // Reset timer
                        m_uiTimeBeforeSleep = 20000;
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void MovementInform(uint32 MovementType, uint32 id) override
    {
        if (MovementType == POINT_MOTION_TYPE && id == 1)
        {
            if (m_uiState == STATE_MOVING_TO_LUMBERPILE)
            {
                m_uiState = STATE_WORKING;
                if (GameObject* LumberPile = m_creature->FindNearestGameObject(GO_LUMBERPILE, 20.0f))
                {
                    m_creature->SetFacingToObject(LumberPile);
                }
                m_creature->HandleEmoteState(EMOTE_WORKING);
            }
            else
            {
                m_uiState = STATE_SLEEPING;
            }
        }
    }
};

CreatureAI* GetAI_LazyPeon(Creature* pCreature)
{
    return new LazyPeonAI(pCreature);
}

void AddSC_durotar()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "LazyPeons";
    newscript->GetAI = &GetAI_LazyPeon;
    newscript->RegisterSelf();
}
