/*
** EPITECH PROJECT, 2025
** zappy [WSL: Ubuntu]
** File description:
** player_death.c
*/

#include "server.h"

bool player_consume_food(player_t *player, uint64_t current_time, int freq)
{
    uint64_t time_since_meal;
    uint64_t food_consumption_interval;

    if (!player || player->is_dead)
        return false;
    time_since_meal = current_time - player->last_meal;
    food_consumption_interval = (126 * 1000) / freq;
    if (time_since_meal >= food_consumption_interval) {
        if (player->inventory[FOOD] > 0) {
            player->inventory[FOOD]--;
            player->last_meal = current_time;
            return true;
        } else {
            return false;
        }
    }
    return true;
}

bool player_consume_food_and_notify(player_t *player, uint64_t current_time,
    int freq, bool *food_consumed)
{
    uint64_t time_since_meal;
    uint64_t food_consumption_interval;

    if (food_consumed)
        *food_consumed = false;
    if (!player || player->is_dead)
        return false;
    time_since_meal = current_time - player->last_meal;
    food_consumption_interval = (126 * 1000) / freq;
    if (time_since_meal < food_consumption_interval)
        return true;
    if (player->inventory[FOOD] > 0) {
        player->inventory[FOOD]--;
        player->last_meal = current_time;
        if (food_consumed)
            *food_consumed = true;
        return true;
    } else {
        return false;
    }
}

bool player_check_death(player_t *player)
{
    if (!player || player->is_dead)
        return player->is_dead;
    if (player->inventory[FOOD] <= 0) {
        player->is_dead = true;
        return true;
    }
    return false;
}

void player_kill(player_t *player)
{
    if (player) {
        player->is_dead = true;
        player->inventory[FOOD] = 0;
    }
}
