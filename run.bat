@echo off
l2-cv-bot.exe ^

    %= NPC detection =%                 ^
    --npc_name_min_height       10      ^
    --npc_name_max_height       18      ^
    --npc_name_min_width        30      ^
    --npc_name_max_width        250     ^
    --npc_name_color_from_hsv   0,0,240 ^
    --npc_name_color_to_hsv     0,0,255 ^
    --npc_name_color_threshold  0.2     ^
    --npc_name_center_offset    15      ^

    %= my HP/MP/CP bars detection =%    ^
    --my_bar_min_height     10          ^
    --my_bar_max_height     20          ^
    --my_bar_min_width      140         ^
    --my_bar_max_width      400         ^
    --my_hp_color_from_hsv  2,90,120    ^
    --my_hp_color_to_hsv    5,220,170   ^
    --my_mp_color_from_hsv  105,100,130 ^
    --my_mp_color_to_hsv    110,255,170 ^
    --my_cp_color_from_hsv  16,100,120  ^
    --my_cp_color_to_hsv    22,255,200  ^

    %= target HP bar detection =%           ^
    --target_hp_min_height      3           ^
    --target_hp_max_height      7           ^
    --target_hp_min_width       140         ^
    --target_hp_max_width       400         ^
    --target_hp_color_from_hsv  0,60,80     ^
    --target_hp_color_to_hsv    2,220,170   ^

    --debug true    ^
    --window %1     ^
