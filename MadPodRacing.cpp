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

class track
{
private:
    struct track_state
    {
        int x;
        int y;
        int next_checkpoint_x; // x position of the next check point
        int next_checkpoint_y; // y position of the next check point
        int next_checkpoint_dist; // distance to the next checkpoint
        int next_checkpoint_angle; // angle between your pod orientation and the direction of the next checkpoint 
    };

private:
    std::vector<std::pair<int, int>> m_checkpoints;
    track_state m_current_state;
    track_state m_previous_state;
    int m_checkpoint_index;
    int last_point_x;
    int last_point_y;
    int current_point_x;
    int current_point_y;
    int next_goal;
    bool m_first_round = true;
    bool m_boosted = false;
    int m_boost_index;

    float BREAK_DISTANCE = 1500.0f;
    float TARGET_DISTANCE = 500.0f;
    float CHECK_ANGLE = 2.0f;
    float PI = 3.14159265f;

public:
    void UpdateStatus(int x, int y, int next_checkpoint_x, int next_checkpoint_y, int next_checkpoint_dist, int next_checkpoint_angle)
    {
        m_previous_state = m_current_state;

        m_current_state.x = x;
        m_current_state.y = y;
        m_current_state.next_checkpoint_x = next_checkpoint_x;
        m_current_state.next_checkpoint_y = next_checkpoint_y;
        m_current_state.next_checkpoint_dist = next_checkpoint_dist;
        m_current_state.next_checkpoint_angle = next_checkpoint_angle;

        SetCheckpoint();
    }

    bool IsNextCheckpoint() const
    {
        return m_checkpoints[m_checkpoint_index].first != m_current_state.next_checkpoint_x && m_checkpoints[m_checkpoint_index].second != m_current_state.next_checkpoint_y;
    }

    int Distance(const pair<int, int>& start, const pair<int, int>& end) const
    {
        int delta_x = (end.first - start.first);
        int delta_y = (end.second - start.second);

        // TODO: Change to use just square values, not squareroot for optimization
        return sqrt(delta_x * delta_x + delta_y * delta_y);
    }

    int CalculateBoostIndex()
    {
        int max_distance = 0.0f;
        int index;

        for (int i = 0, max_i = m_checkpoints.size(); i < max_i; ++i)
        {
            int next_i = i + 1;
            if (next_i == max_i)
            {
                next_i = 0;
            }

            int distance = Distance(m_checkpoints[i], m_checkpoints[next_i]);
            if (distance > max_distance)
            {
                max_distance = distance;
                index = next_i;
            }
        }

        return index;
    }

    void SetCheckpoint()
    {
        if (m_checkpoints.empty())
        {
            m_checkpoints.push_back(pair<int, int>(m_current_state.next_checkpoint_x, m_current_state.next_checkpoint_y));
            m_checkpoint_index = 0;
        }
        else
        {
            if (IsNextCheckpoint())
            {
                if (m_first_round)
                {
                    if (m_checkpoints[0].first == m_current_state.next_checkpoint_x && m_checkpoints[0].second == m_current_state.next_checkpoint_y)
                    {
                        m_first_round = false;
                        m_checkpoint_index = 0;
                        m_boost_index = CalculateBoostIndex();
                    }
                    else
                    {
                        m_checkpoints.push_back(pair<int, int>(m_current_state.next_checkpoint_x, m_current_state.next_checkpoint_y));
                        ++m_checkpoint_index;
                    }
                }
                else
                {
                    ++m_checkpoint_index;
                    if (m_checkpoint_index >= m_checkpoints.size())
                    {
                        m_checkpoint_index = 0;
                    }
                }
            }
        }
    }

    int GetPreviousCheckpointIndex()
    {
        int index = m_checkpoint_index - 1;
        if (m_checkpoint_index < 0)
        {
            m_checkpoint_index = m_checkpoints.size() - 1;
        }

        return index;
    }

    string GetOutputString()
    {
        float delta_x = (m_current_state.next_checkpoint_x - m_current_state.x);
        float delta_y = (m_current_state.next_checkpoint_y - m_current_state.y);

        // float angle = atan (delta_y / delta_x);
        // float distance = sqrt(delta_y * delta_y + delta_x * delta_x);

        pair<int, int>& last_checkpoint = m_checkpoints[GetPreviousCheckpointIndex()];

        float delta_lastpoint_x = (last_checkpoint.first - m_current_state.x);
        float delta_lastpoint_y = (last_checkpoint.second - m_current_state.y);

        float distance_last_point = sqrt(delta_lastpoint_y * delta_lastpoint_y + delta_lastpoint_x * delta_lastpoint_x);

        float x_multiplier = delta_x < 0.0f ? -1 : 1;
        float y_multiplier = delta_y < 0.0f ? -1 : 1;

        //cerr << "angle: " <<  angle * 180.0f / PI << "  distance: " << distance << endl;
        // cerr << "multipliers: (" << x_multiplier << ", " << y_multiplier << ")" << endl;
        /*
        float goal_distance = distance - TARGET_DISTANCE;
        float dir_x = cos(angle) * goal_distance;
        float dir_y = sin(angle) * goal_distance;
        float goal_x = x + (x_multiplier) * abs(dir_x);
        float goal_y = y + (y_multiplier) * abs(dir_y);
        */

        //cerr << "Goal: (" << goal_x << ", " << goal_y << "), dir: (" << dir_x << ", " << dir_y << ")" << endl;

        int thrust = 100;

        if (abs(m_current_state.next_checkpoint_angle) > 90)
        {
            cerr << "====STOP:" << endl;
            thrust = 0;
        }
        else if (m_current_state.next_checkpoint_dist < BREAK_DISTANCE)
        {
            thrust = lerp(0, 100, (float)abs(m_current_state.next_checkpoint_dist) / BREAK_DISTANCE);
            cerr << "####BREAK distance:" << m_current_state.next_checkpoint_dist << endl;
        }
        /*        else if(distance_last_point < BREAK_DISTANCE && abs(next_checkpoint_angle) > CHECK_ANGLE))
                {
                    int min_dist = next_checkpoint_dist < distance_last_point ? next_checkpoint_dist : distance_last_point;
                    if(min_dist > BREAK_DISTANCE)
                    {
                        min_dist = BREAK_DISTANCE;
                    }
                }
        */
        else if (abs(m_current_state.next_checkpoint_angle) > CHECK_ANGLE)
        {
            float abs_angle = (float)abs(m_current_state.next_checkpoint_angle);
            thrust = lerp(100, 0, abs_angle / 90.0f);
            cerr << "####BREAK angle:" << abs_angle << " thrust: " << thrust << endl;
        }
        else
        {
            thrust = 100;
            cerr << "***GOGOGO':" << endl;
        }

        cerr << " dist:" << m_current_state.next_checkpoint_dist << " dist_LP: " << distance_last_point << endl;
        cerr << "angle:" << m_current_state.next_checkpoint_angle << " thrust:" << thrust << endl;

        string output;

        if (!m_first_round && !m_boosted && m_checkpoint_index == m_boost_index && thrust == 100 && m_current_state.next_checkpoint_angle < CHECK_ANGLE)
        {
            m_boosted = true;
            output = std::to_string((int)(m_current_state.next_checkpoint_x)) + " " + std::to_string((int)(m_current_state.next_checkpoint_y)) + " BOOST";
        }
        else
        {
            output = std::to_string((int)(m_current_state.next_checkpoint_x)) + " " + std::to_string((int)(m_current_state.next_checkpoint_y)) + " " + std::to_string(thrust);
        }

        return output;
    }
};

int main()
{
    track* m_track = new track();

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

        m_track->UpdateStatus(x, y, next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, next_checkpoint_angle);

        cout << m_track->GetOutputString() << endl;

    }
}