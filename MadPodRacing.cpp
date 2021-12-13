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

class Point
{
public:
    int x;
    int y;

    Point()
    {
        x = 0;
        y = 0;
    }

    Point(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    void Set(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    Point& operator=(const Point& rhs)
    {
        if (this == &rhs)
            return *this;

        x = rhs.x;
        y = rhs.y;

        return *this;
    }

    int Distance(const Point& other) const
    {
        int delta_x = (x - other.x);
        int delta_y = (y - other.y);

        // TODO: Optimization, don't do square root, but keeping it for now for easier number comparison
        return sqrt(delta_x * delta_x + delta_y * delta_y);
    }

    Point operator+(const Point& other)
    {
        return Point(x + other.x, y + other.y);
    }

    Point operator-(const Point& other)
    {
        return Point(x - other.x, y - other.y);
    }

    friend ostream& operator<<(ostream& os, const Point& point);
    friend bool operator== (const Point& point1, const Point& point2);
    friend bool operator!= (const Point& point1, const Point& point2);
};

ostream& operator<<(ostream& os, const Point& point)
{
    os << "(" << point.x << ", " << point.y << ")";
    return os;
}

bool operator== (const Point& point1, const Point& point2)
{
    return (point1.x == point2.x && point1.y == point2.y);
}

bool operator!= (const Point& point1, const Point& point2)
{
    return !(point1 == point2);
}

class TrackState
{
public:
    Point position;             // Pod position
    Point next_checkpoint;      // position of the next check point
    int next_checkpoint_dist;   // distance to the next checkpoint
    int next_checkpoint_angle;  // angle between your pod orientation and the direction of the next checkpoint

    TrackState()
    {
        position = Point(0, 0);
        next_checkpoint = Point(0, 0);
    }

    TrackState(int _x, int _y, int _next_checkpoint_x, int _next_checkpoint_y, int _next_checkpoint_dist, int _next_checkpoint_angle)
    {
        position = Point(_x, _y);
        next_checkpoint = Point(_next_checkpoint_x, _next_checkpoint_y);
        next_checkpoint_dist = _next_checkpoint_dist;
        next_checkpoint_angle = _next_checkpoint_angle;
    }
};

class Track
{
private:
    std::vector<Point> m_checkpoints;
    TrackState* m_current_state = nullptr;
    TrackState* m_previous_state = nullptr;;
    int m_checkpoint_index = 0;
    bool m_first_round = true;
    bool m_boosted = false;
    int m_boost_index = 0;
    bool m_next_checkpoint_selected = false;

    // Constants
    float BREAK_DISTANCE = 1500.0f;
    float TARGET_DISTANCE = 500.0f;
    float CHECK_ANGLE = 2.0f;
    int HIT_SPEED = 500;
    float PI = 3.14159265f;

public:
    ~Track()
    {
        if (m_current_state != nullptr)
        {
            delete m_current_state;
        }

        if (m_previous_state != nullptr)
        {
            delete m_previous_state;
        }
    }

    void UpdateStatus(int x, int y, int next_checkpoint_x, int next_checkpoint_y, int next_checkpoint_dist, int next_checkpoint_angle)
    {
        if (m_previous_state != nullptr)
        {
            delete m_previous_state;
        }

        m_previous_state = m_current_state;

        m_current_state = new TrackState(x, y, next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, next_checkpoint_angle);

        SetCheckpoint();
    }

    bool IsNextCheckpoint() const
    {
        return m_checkpoints[m_checkpoint_index] != m_current_state->next_checkpoint;
    }

    int CalculateBoostIndex()
    {
        int max_distance = 0.0f;
        int index = 0;

        for (int i = 0, max_i = m_checkpoints.size(); i < max_i; ++i)
        {
            int next_i = i + 1;
            if (next_i == max_i)
            {
                next_i = 0;
            }

            int distance = m_checkpoints[i].Distance(m_checkpoints[next_i]);
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
            m_checkpoints.push_back(m_current_state->next_checkpoint);
            m_checkpoint_index = 0;
        }
        else
        {
            if (IsNextCheckpoint())
            {
                if (m_first_round)
                {
                    if (m_checkpoints[0] == m_current_state->next_checkpoint)
                    {
                        m_first_round = false;
                        m_checkpoint_index = 0;
                        m_boost_index = CalculateBoostIndex();
                    }
                    else
                    {
                        m_checkpoints.push_back(m_current_state->next_checkpoint);
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

                    m_next_checkpoint_selected = false;
                }
            }
        }
    }

    int GetPreviousCheckpointIndex() const
    {
        int index = m_checkpoint_index - 1;
        if (index < 0)
        {
            index = m_checkpoints.size() - 1;
        }

        return index;
    }

    int GetNextCheckpointIndex() const
    {
        int index = m_checkpoint_index + 1;
        if (index == m_checkpoints.size())
        {
            index = 0;
        }

        return index;
    }

    Point GetNextCheckpoint() const
    {
        return m_checkpoints[GetNextCheckpointIndex()];
    }

    float GetSpeed() const
    {
        if (m_previous_state == nullptr)
        {
            return 0.0f;
        }

        return m_current_state->position.Distance(m_previous_state->position);
    }

    bool PredictCheckpointHit(int speed, int angle) const
    {
        string first_round = m_first_round ? "No" : "OK";
        string speed_txt = speed > HIT_SPEED ? "OK" : "No";
        string angle_txt = angle < CHECK_ANGLE ? "OK" : "No";
        string distance_txt = m_current_state->next_checkpoint_dist < BREAK_DISTANCE ? "OK" : "No";

        cerr << "Predict first: " << first_round << " Speed: " << speed_txt << " Angle: " << angle_txt << " Distance: " << distance_txt << endl;
        return !m_first_round && speed > HIT_SPEED && angle < CHECK_ANGLE&& m_current_state->next_checkpoint_dist < 2000;
    }

    string GetOutputString()
    {
        Point delta = m_current_state->next_checkpoint - m_current_state->position;

        // float angle = atan (delta_y / delta_x);
        // float distance = sqrt(delta_y * delta_y + delta_x * delta_x);

        // Point last_checkpoint = m_checkpoints[GetPreviousCheckpointIndex()];

        // float distance_last_point = last_checkpoint.Distance(m_current_state->position);

        float x_multiplier = delta.x < 0.0f ? -1 : 1;
        float y_multiplier = delta.y < 0.0f ? -1 : 1;

        // cerr << "angle: " <<  angle * 180.0f / PI << "  distance: " << distance << endl;
        // cerr << "multipliers: (" << x_multiplier << ", " << y_multiplier << ")" << endl;
        /*
        float goal_distance = distance - TARGET_DISTANCE;
        float dir_x = cos(angle) * goal_distance;
        float dir_y = sin(angle) * goal_distance;
        float goal_x = x + (x_multiplier) * abs(dir_x);
        float goal_y = y + (y_multiplier) * abs(dir_y);
        */

        // cerr << "Goal: (" << goal_x << ", " << goal_y << "), dir: (" << dir_x << ", " << dir_y << ")" << endl;

        int thrust = 100;
        int speed = GetSpeed();

        int angle = abs(m_current_state->next_checkpoint_angle);
        Point goToPoint = m_current_state->next_checkpoint;

        if (m_next_checkpoint_selected || PredictCheckpointHit(speed, angle))
        {
            cerr << "Go to next!!!" << endl;

            m_next_checkpoint_selected = true;
            goToPoint = GetNextCheckpoint();
            thrust = 20;
        }
        else if (angle > 90)
        {
            cerr << "====STOP:" << endl;
            thrust = 0;
        }
        else if (m_current_state->next_checkpoint_dist < BREAK_DISTANCE)
        {
            thrust = lerp(0, 100, (float)abs(m_current_state->next_checkpoint_dist) / BREAK_DISTANCE);
            cerr << "####BREAK distance:" << m_current_state->next_checkpoint_dist << endl;
        }
        else if (angle > CHECK_ANGLE)
        {
            float abs_angle = (float)angle;
            thrust = lerp(100, 0, abs_angle / 90.0f);
            cerr << "####BREAK angle:" << abs_angle << " thrust: " << thrust << endl;
        }
        else
        {
            thrust = 100;
            cerr << "***GOGOGO':" << endl;
        }

        cerr << "Speed: " << speed << "angle:" << m_current_state->next_checkpoint_angle << " dist:" << m_current_state->next_checkpoint_dist << endl;
        // cerr << " dist:" << m_current_state->next_checkpoint_dist << " dist_LP: " << distance_last_point << endl;
        // cerr << "angle:" << m_current_state->next_checkpoint_angle << " thrust:" << thrust << endl;

        string output;

        if (!m_first_round && !m_boosted && m_checkpoint_index == m_boost_index && thrust == 100 && abs(m_current_state->next_checkpoint_angle) < CHECK_ANGLE)
        {
            m_boosted = true;
            output = std::to_string(goToPoint.x) + " " + std::to_string(goToPoint.y) + " BOOST";
        }
        else
        {
            output = std::to_string(goToPoint.x) + " " + std::to_string(goToPoint.y) + " " + std::to_string(thrust);
        }

        return output;
    }
};

int main()
{
    Track* m_track = new Track();

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

        // cerr << "input: next_checkpoint: (" << next_checkpoint_x << ", " << next_checkpoint_y << ")" << endl;
        m_track->UpdateStatus(x, y, next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, next_checkpoint_angle);

        cout << m_track->GetOutputString() << endl;
    }

    delete m_track;
}