#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

int main()
{
    int thrust = 100;
    bool boosted = false;
    int last_distance = 0;
    float BREAK_DISTANCE = 800.0f;
    int TARGET_DISTANCE = 500;
    int CHECK_ANGLE = 5;
    float PI = 3.14159265f;
    bool apex_reached = false;
    int last_point_x;
    int last_point_y;
    int current_point_x;
    int current_point_y;
    std::vector<std::pair<int, int>> goals;
    int next_goal;
    bool goals_collected = false;

    // game loop
    while (1) {
        int x;
        int y;
        int next_checkpoint_x; // x position of the next check point
        int next_checkpoint_y; // y position of the next check point
        int next_checkpoint_dist; // distance to the next checkpoint
        int next_checkpoint_angle; // angle between your pod orientation and the direction of the next checkpoint
        cin >> x >> y >> next_checkpoint_x >> next_checkpoint_y >> next_checkpoint_dist >> next_checkpoint_angle; cin.ignore();
        int opponent_x;
        int opponent_y;
        cin >> opponent_x >> opponent_y; cin.ignore();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"

        if (last_point_x == 0)
        {
            last_point_x = x;
            last_point_y = y;

            current_point_x = next_checkpoint_x;
            current_point_y = next_checkpoint_y;

            goals.push_back(pair<int, int>(next_checkpoint_x, next_checkpoint_y));
        }
        else if (current_point_x != next_checkpoint_x)
        {
            last_point_x = current_point_x;
            last_point_y = current_point_y;

            current_point_x = next_checkpoint_x;
            current_point_y = next_checkpoint_y;

            if (!goals.empty() && goals[0].first == next_checkpoint_x)
            {
                goals_collected = true;
            }

            if (!goals_collected)
            {
                goals.push_back(pair<int, int>(next_checkpoint_x, next_checkpoint_y));
            }
            else if (goals[1].first == next_checkpoint_x)
            {
                boosted = false;
            }
        }

        float delta_x = (next_checkpoint_x - x);
        float delta_y = (next_checkpoint_y - y);

        //cerr << "Pos: (" << x << ", " << y << ")" << endl;
        //cerr << "Checkpoint: (" << next_checkpoint_x << ", " << next_checkpoint_y << ")" << endl;
        //cerr << "Delta: (" << delta_x << ", " << delta_y << ")" << endl;

        float angle = atan(delta_y / delta_x);
        float distance = sqrt(delta_y * delta_y + delta_x * delta_x);

        float delta_lastpoint_x = (last_point_x - x);
        float delta_lastpoint_y = (last_point_y - y);
        float distance_last_point = sqrt(delta_lastpoint_y * delta_lastpoint_y + delta_lastpoint_x * delta_lastpoint_x);

        float x_multiplier = delta_x < 0.0f ? -1 : 1;
        float y_multiplier = delta_y < 0.0f ? -1 : 1;

        //cerr << "angle: " <<  angle * 180.0f / PI << "  distance: " << distance << endl;
        // cerr << "multipliers: (" << x_multiplier << ", " << y_multiplier << ")" << endl;
        float goal_distance = distance - TARGET_DISTANCE;
        float dir_x = cos(angle) * goal_distance;
        float dir_y = sin(angle) * goal_distance;
        float goal_x = x + (x_multiplier)*abs(dir_x);
        float goal_y = y + (y_multiplier)*abs(dir_y);

        //cerr << "Goal: (" << goal_x << ", " << goal_y << "), dir: (" << dir_x << ", " << dir_y << ")" << endl;

        if (abs(next_checkpoint_angle) > 90)
        {
            cerr << "====STOP:" << endl;
            thrust = 0;
        }
        else if ((next_checkpoint_dist < BREAK_DISTANCE || distance_last_point < BREAK_DISTANCE) && abs(next_checkpoint_angle) > CHECK_ANGLE)
        {
            int min_dist = next_checkpoint_dist < distance_last_point ? next_checkpoint_dist : distance_last_point;
            if (min_dist > BREAK_DISTANCE)
            {
                min_dist = BREAK_DISTANCE;
            }
            thrust = lerp(100, 0, (float)min_dist / BREAK_DISTANCE);
            cerr << "####BREAK:" << endl;
        }
        else
        {
            thrust = 100;
            cerr << "***GOGOGO':" << endl;
        }

        cerr << "angle:" << next_checkpoint_angle << " dist:" << next_checkpoint_dist << " distance_last_point" << distance_last_point << " thrust:" << thrust << endl;

        if (!boosted && next_checkpoint_dist > 4000 && thrust == 100 && next_checkpoint_angle == 0)
        {
            boosted = true;
            cout << (int)goal_x << " " << (int)goal_y << " " << "BOOST" << endl;
        }
        else
        {
            cout << (int)next_checkpoint_x << " " << (int)next_checkpoint_y << " " << thrust << endl;
        }

        last_distance = next_checkpoint_dist;
    }
}