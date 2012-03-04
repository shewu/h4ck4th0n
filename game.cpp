#include "game.h"
#include "constants.h"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

Game::Game() {
    team_count[RED] = 0;
    team_count[BLU] = 0;
}

bool Game::add_player(Client cl) {
    if(players.find(cl.id) != players.end())
        return false;

    Player player;

    player.cl = cl;

    player.object_id = -1;
    player.key_pressed = 0;
    player.time_until_spawn = SPAWN_TIME;
    player.angle = (rand() / float(RAND_MAX)) * 2 * M_PI;

    if(team_count[RED] == team_count[BLU])
        player.team = (rand() % 2 ? BLU : RED);
    else
        player.team = (team_count[RED] > team_count[BLU] ? BLU : RED);
    team_count[player.team]++;

    players[cl.id] = player;

    WritePacket wp(STC_INITIAL_ANGLE, 4);
    wp.write_float(player.angle);
    cl.sc->send_packet(wp);

    return true;
}

void Game::remove_player(int id) {
    if(players.find(id) == players.end())
        return;

    team_count[players[id].team]--;

    Player p = players[id];
    if(p.object_id != -1) {
        if(!world.objects[p.object_id].dead)
            world.objects.erase(p.object_id);
        else
            world.objects[p.object_id].player = -2;
    }

    players.erase(id);
}

void Game::send_world() {
    for(map<int, Player>::iterator p = players.begin();
            p != players.end(); ++p) {
        world.sendObjects(p->second.cl.sc, p->second.object_id);
        send_sounds_to(p->second.cl.sc);
    }
}

void Game::send_sounds_to(SocketConnection* sc) {
	for(vector<pair<char, Vector2D> >::iterator it = sounds.begin();
            it != sounds.end(); ++it) {
        WritePacket wp(STC_SOUND, 9);
        wp.write_char(it->first);
        wp.write_float(it->second.x);
        wp.write_float(it->second.y);
        sc->send_packet(wp);
    }
}

void Game::process_packet(int id, ReadPacket* rp) {
    if(players.count(id) == 0)
        return;

    if(rp->message_type == CTS_USER_STATE) {
        if(rp->size != 5)
            return;
        players[id].angle = rp->read_float();
        players[id].key_pressed = rp->read_char();
    }
}

void Game::update(float dt) {
    for(map<int, Player>::iterator p = players.begin();
            p != players.end(); ++p) {

        if(p->second.object_id == -1) {
            p->second.time_until_spawn -= dt;
            while(p->second.object_id == -1 &&
                    p->second.time_until_spawn < 0.0) {
                p->second.object_id = world.spawn(2 * p->second.team,
                        p->second.cl.id, NO_TEAM); //wtf does this mean?
                if(p->second.object_id == -1)
                    p->second.time_until_spawn += SPAWN_TRY;
            }
        }

        Vector2D acceleration = Vector2D(0.0f, 0.0f);
        int value = 0;

        if(p->second.key_pressed & 1) {
            acceleration += Vector2D(cos(p->second.angle),
                    sin(p->second.angle));
            value += 1;
        }
        if(p->second.key_pressed & 2) {
            acceleration += Vector2D(-cos(p->second.angle),
                    -sin(p->second.angle));
            value -= 1;
        }
        if(p->second.key_pressed & 4) {
            acceleration += Vector2D(sin(p->second.angle),
                    -cos(p->second.angle));
            value += 2;
        }
        if(p->second.key_pressed & 8) {
            acceleration += Vector2D(-sin(p->second.angle),
                    cos(p->second.angle));
            value -= 2;
        }
        if(value != 0) {
            acceleration = acceleration.getNormalVector() *
                KEYPRESS_ACCELERATION;
            if(p->second.object_id != -1 &&
                    !world.objects[p->second.object_id].dead)
                world.objects[p->second.object_id].v += acceleration * dt;
        }
    }

    for(map<int, Object>::iterator it = world.objects.begin();
            it != world.objects.end();) {
        map<int, Object>::iterator next_it = it;
        ++next_it;

        if(!it->second.dead) {
            it->second.v -= it->second.v * FRICTION * dt;
        }                                                                      
        else if(it->second.spawny == 0) {
            it->second.spawny = 1;
            it->second.time_until_spawn = SPAWN_TIME;
        }
        else if(it->second.spawny == 1) {
            if(it->second.player == -2)
                it->second.spawny = 2;
            else {
                it->second.time_until_spawn -= dt;
                while(it->second.spawny == 1 &&
                        it->second.time_until_spawn < 0.0) {
                    int newo = world.spawn(it->second.spawnl,
                            it->second.player, it->second.flag);
                    if(newo == -1) {
                        it->second.time_until_spawn += SPAWN_TRY;
                    }
                    else {
                        it->second.spawny = 2;
                        if(it->second.player != -1) {
                            players[it->second.player].object_id = newo;
                            it->second.player = -1;
                        }
                    }
                }
            }
        }
        else if(it->second.stopped) {
            world.objects.erase(it);
        }

        it = next_it;
    }

	sounds.clear();
    world.doSimulation(dt, sounds);
}
