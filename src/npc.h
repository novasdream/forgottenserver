/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2014  Mark Samman <mark.samman@gmail.com>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_NPC_H_B090D0CB549D4435AFA03647195D156F
#define FS_NPC_H_B090D0CB549D4435AFA03647195D156F

#include "creature.h"
#include "luascript.h"

#include <set>

class Npc;
class Player;

class Npcs
{
	public:
		static void reload();
};

class NpcScriptInterface final : public LuaScriptInterface
{
	public:
		NpcScriptInterface();
		~NpcScriptInterface();

		bool loadNpcLib(const std::string& file);

	protected:
		void registerFunctions();

		static int32_t luaActionSay(lua_State* L);
		static int32_t luaActionMove(lua_State* L);
		static int32_t luaActionMoveTo(lua_State* L);
		static int32_t luaActionTurn(lua_State* L);
		static int32_t luaActionFollow(lua_State* L);
		static int32_t luaSelfGetPos(lua_State* L);
		static int32_t luagetDistanceTo(lua_State* L);
		static int32_t luaSetNpcFocus(lua_State* L);
		static int32_t luaGetNpcCid(lua_State* L);
		static int32_t luaGetNpcPos(lua_State* L);
		static int32_t luaGetNpcName(lua_State* L);
		static int32_t luaGetNpcParameter(lua_State* L);
		static int32_t luaOpenShopWindow(lua_State* L);
		static int32_t luaCloseShopWindow(lua_State* L);
		static int32_t luaDoSellItem(lua_State* L);

		// metatable
		static int32_t luaNpcGetParameter(lua_State* L);
		static int32_t luaNpcSetFocus(lua_State* L);

		static int32_t luaNpcOpenShopWindow(lua_State* L);
		static int32_t luaNpcCloseShopWindow(lua_State* L);

	private:
		bool initState() final;
		bool closeState() final;

		bool m_libLoaded;
};

class NpcEventsHandler
{
	public:
		NpcEventsHandler(const std::string& file, Npc* npc);
		~NpcEventsHandler();

		void onCreatureAppear(Creature* creature);
		void onCreatureDisappear(Creature* creature);
		void onCreatureMove(Creature* creature, const Position& oldPos, const Position& newPos);
		void onCreatureSay(Creature* creature, SpeakClasses, const std::string& text);
		void onPlayerTrade(Player* player, int32_t callback, uint16_t itemId, uint8_t count, uint8_t amount, bool ignore = false, bool inBackpacks = false);
		void onPlayerCloseChannel(Player* player);
		void onPlayerEndTrade(Player* player);
		void onThink();

		bool isLoaded() const;

	protected:
		Npc* m_npc;
		NpcScriptInterface* m_scriptInterface;

		int32_t m_onCreatureAppear;
		int32_t m_onCreatureDisappear;
		int32_t m_onCreatureMove;
		int32_t m_onCreatureSay;
		int32_t m_onPlayerCloseChannel;
		int32_t m_onPlayerEndTrade;
		int32_t m_onThink;
		bool m_loaded;
};

class Npc final : public Creature
{
	public:
		~Npc();

		Npc* getNpc() final {
			return this;
		}
		const Npc* getNpc() const final {
			return this;
		}

		bool isPushable() const final {
			return walkTicks > 0;
		}

		void setID() final {
			if (id == 0) {
				id = npcAutoID++;
			}
		}

		void removeList() final;
		void addList() final;

		static Npc* createNpc(const std::string& name);

		bool canSee(const Position& pos) const final;

		bool load();
		void reload();

		const std::string& getName() const final {
			return name;
		}
		const std::string& getNameDescription() const final {
			return name;
		}

		CreatureType_t getType() const final {
			return CREATURETYPE_NPC;
		}

		uint8_t getSpeechBubble() const final {
			return speechBubble;
		}
		void setSpeechBubble(const uint8_t bubble) {
			speechBubble = bubble;
		}

		void doSay(const std::string& text);
		void doSayToPlayer(Player* player, const std::string& text);

		void doMove(Direction dir);
		void doTurn(Direction dir);
		void doMoveTo(const Position& pos);

		int32_t getMasterRadius() const {
			return masterRadius;
		}
		const Position& getMasterPos() const {
			return masterPos;
		}
		void setMasterPos(const Position& pos, int32_t radius = 1) {
			masterPos = pos;
			if (masterRadius == -1) {
				masterRadius = radius;
			}
		}

		void onPlayerCloseChannel(Player* player);
		void onPlayerTrade(Player* player, int32_t callback, uint16_t itemId, uint8_t count,
		                   uint8_t amount, bool ignore = false, bool inBackpacks = false);
		void onPlayerEndTrade(Player* player, int32_t buyCallback, int32_t sellCallback);

		void turnToCreature(Creature* creature);
		void setCreatureFocus(Creature* creature);

		NpcScriptInterface* getScriptInterface();

		static uint32_t npcAutoID;

	protected:
		Npc(const std::string& _name);

		void onCreatureAppear(Creature* creature, bool isLogin) final;
		void onCreatureDisappear(Creature* creature, uint32_t stackpos, bool isLogout) final;
		void onCreatureMove(Creature* creature, const Tile* newTile, const Position& newPos,
		                            const Tile* oldTile, const Position& oldPos, bool teleport) final;

		void onCreatureSay(Creature* creature, SpeakClasses type, const std::string& text) final;
		void onThink(uint32_t interval) final;
		std::string getDescription(int32_t lookDistance) const final;

		bool isImmune(CombatType_t) const final {
			return !attackable;
		}
		bool isImmune(ConditionType_t) const final {
			return !attackable;
		}
		bool isAttackable() const final {
			return attackable;
		}
		bool getNextStep(Direction& dir, uint32_t& flags) final;

		bool canWalkTo(const Position& fromPos, Direction dir) const;
		bool getRandomStep(Direction& dir) const;

		void reset();
		bool loadFromXml(const std::string& name);

		void addShopPlayer(Player* player);
		void removeShopPlayer(Player* player);
		void closeAllShopWindows();

		std::map<std::string, std::string> m_parameters;

		std::set<Player*> shopPlayerSet;

		std::string name;
		std::string m_filename;

		NpcEventsHandler* m_npcEventHandler;

		Position masterPos;

		uint32_t walkTicks;
		int32_t focusCreature;
		int32_t masterRadius;

		uint8_t speechBubble;

		bool floorChange;
		bool attackable;
		bool ignoreHeight;
		bool loaded;

		static NpcScriptInterface* m_scriptInterface;

		friend class Npcs;
		friend class NpcScriptInterface;
};

#endif
