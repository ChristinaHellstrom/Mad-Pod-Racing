#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    int thrust = 100;
    bool boosted = false;
    int last_distance = 0;
    int BREAK_DISTANCE = 600;
    int MIN_ANGLE = 10;
    bool apex_reached = false;

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

        if (next_checkpoint_dist < BREAK_DISTANCE /*|| abs(next_checkpoint_angle) > MIN_ANGLE*/)
        {
            thrust = 0;
            cerr << "####BREAK:" << endl;
        }
        else
        {
            thrust = 100;
            cerr << "***GOGOGO':" << endl;
        }

        cerr << "angle:" << next_checkpoint_angle << " dist:" << next_checkpoint_dist << " thrust:" << thrust << endl;

        if (!boosted && next_checkpoint_dist > 4000 && thrust == 100 && next_checkpoint_angle == 0)
        {
            boosted = true;
            cout << next_checkpoint_x << " " << next_checkpoint_y << " " << "BOOST" << endl;
        }
        else
        {
            cout << next_checkpoint_x << " " << next_checkpoint_y << " " << thrust << endl;
        }

        last_distance = next_checkpoint_dist;
    }
}