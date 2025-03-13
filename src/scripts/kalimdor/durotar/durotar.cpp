/*
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
#include "Utilities/EventMap.h"

// ---------------------------
// NPC:   Lazy Peon (id:10556)
// Quest: Lazy Peons
// ---------------------------

enum LazyPeon : uint32
{
    QUEST_LAZY_PEONS  = 5441,
    GO_LUMBERPILE     = 175784,
    SPELL_BUFF_SLEEP  = 17743,
    SPELL_AWAKEN_PEON = 19938,
    EMOTE_WORKING     = 234,
    SAY_HIT           = 5774, // Ow!  OK, I'll get back to work, $n!
    SAY_STOP          = 5056  // Hey!  Stop that!
};

enum Events : uint8
{
    EVENT_SLEEPING    = 1,
    EVENT_WAKE_UP     = 2,
    EVENT_WORK        = 3,
    EVENT_MOVING_BACK = 4,
};

struct LazyPeonAI : public ScriptedAI
{
    explicit LazyPeonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    EventMap m_events;
    bool m_isAboutToWork;

    void Reset() override
    {
        m_isAboutToWork = false;
        m_events.Reset();
        m_events.ScheduleEvent(EVENT_SLEEPING, Milliseconds(500));
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

        m_events.CancelEvent(EVENT_SLEEPING);
        m_events.ScheduleEvent(EVENT_WAKE_UP, Milliseconds(100));
        DoScriptText(SAY_HIT, m_creature, player);

        if (player->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
        {
            player->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetGUID());
        }
    }

    void MovementInform(uint32 MovementType, uint32 id) override
    {
        if (MovementType == POINT_MOTION_TYPE && id == 1)
        {
            m_events.ScheduleEvent(m_isAboutToWork ? EVENT_WORK : EVENT_SLEEPING, Milliseconds(500));
        }
    }

    void UpdateAI(uint32 const diff) override
    {
        m_events.Update(diff);

        while (const uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SLEEPING:
                {
                    if (!m_creature->HasAura(SPELL_BUFF_SLEEP))
                    {
                        m_creature->CastSpell(m_creature, SPELL_BUFF_SLEEP, true);
                    }
                    m_events.Repeat(Minutes(2)); // Loop
                    break;
                }
                case EVENT_WAKE_UP:
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_BUFF_SLEEP);
                    if (GameObject* pLumberPile = m_creature->FindNearestGameObject(GO_LUMBERPILE, 20.0f))
                    {
                        m_isAboutToWork = true;
                        m_creature->SetWalk(false); // run!
                        float fX, fY, fZ;
                        pLumberPile->GetContactPoint(m_creature, fX, fY, fZ, CONTACT_DISTANCE + 0.2f);
                        // Peon should run to work - speed: 5.f
                        m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ, MOVE_PATHFINDING /*, 5.f*/);
                    }
                    break;
                }
                case EVENT_WORK:
                {
                    if (GameObject* pLumberPile = m_creature->FindNearestGameObject(GO_LUMBERPILE, 20.0f))
                    {
                        m_creature->SetFacingToObject(pLumberPile);
                    }
                    m_creature->HandleEmoteState(EMOTE_WORKING);
                    m_events.ScheduleEvent(EVENT_MOVING_BACK, Seconds(20));
                    break;
                }
                case EVENT_MOVING_BACK:
                {
                    m_isAboutToWork = false;
                    m_creature->SetWalk(true); // walk!
                    m_creature->HandleEmoteState(0);
                    float x, y, z, o;
                    m_creature->GetHomePosition(x, y, z, o);
                    m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
                }
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
    Script* newscript = new Script;
    newscript->Name = "LazyPeons";
    newscript->GetAI = &GetAI_LazyPeon;
    newscript->RegisterSelf();
}
