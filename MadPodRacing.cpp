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

//
// Class for a point (x, y)
//
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

    Point(const int _x, const int _y)
    {
        x = _x;
        y = _y;
    }

    void Set(const int _x, const int _y)
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

    //
    // Distance to another point
    //
    int Distance(const Point& other) const
    {
        int delta_x = (x - other.x);
        int delta_y = (y - other.y);

        // TODO: Optimization, don't do square root, but keeping it for now for easier number comparison
        return sqrt(delta_x * delta_x + delta_y * delta_y);
    }

    Point operator+(const Point& other) const
    {
        return Point(x + other.x, y + other.y);
    }

    Point operator-(const Point& other) const
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

//
// Data of a round
//
struct TrackState
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
    std::vector<Point> m_checkpoints;               // List of checkpoints. Collected during the first round
    TrackState* m_current_state = nullptr;          // Data for the state of the current round
    TrackState* m_previous_state = nullptr;         // Data for the state of the previous round
    int m_checkpoint_index = 0;                     // What checkpoint are we currently at?
    bool m_first_round = true;                      // Are we on the first round?
    bool m_boosted = false;                         // Have we already boosted?
    int m_boost_index = 0;                          // At what checkpoint do we want to boost?
    bool m_next_checkpoint_selected = false;        // Used when we have started to move in advance to the next checkpoint 
                                                    // Are we moving to the next checkpoint before the game has selected it?
    //
    // Constants
    //
    float BREAK_DISTANCE = 1200.0f;                 // At what distance do we start to slow the thrust
    int PREDICT_DISTANCE = 2000;                    // At what distance do we start to check if we can start moving to the next checkpoint
    float CHECK_ANGLE = 3.0f;                       // Angle that is "straight enough"
    int HIT_SPEED = 500;                            // What speed is fast enough to hit the target if we start moving to another direction
    float PI = 3.14159265f;                         // Pi!

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

    //
    // Set incoming data for the current round into our own state structure. 
    // Move the data we had to be the data for the previous round.
    //
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

    //
    // Is the input data saying that we are going to the next checkpoint?
    //
    bool IsNextCheckpoint() const
    {
        return m_checkpoints[m_checkpoint_index] != m_current_state->next_checkpoint;
    }

    //
    // Calculate what checkpoint index has the longest distance to the next checkpoint
    //
    int CalculateBoostIndex() const
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

    //
    // First round: Collect the checkpoints into a list
    // Following rounds: Update the current checkpoint index
    //
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

    //
    // Index of the previous checkpoint in the list of checkpoints
    //
    int GetPreviousCheckpointIndex() const
    {
        int index = m_checkpoint_index - 1;
        if (index < 0)
        {
            index = m_checkpoints.size() - 1;
        }

        return index;
    }

    //
    // Index of the next checkpoint in the list of checkpoints
    //
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

    //
    // Speed is the distance between our current and last positions
    //
    float GetSpeed() const
    {
        if (m_previous_state == nullptr)
        {
            return 0.0f;
        }

        return m_current_state->position.Distance(m_previous_state->position);
    }

    //
    // Predict if we have enough speed and correct angle to hit the checkpoint if we start moving to the next one in advance
    //
    bool PredictCheckpointHit(const int speed, const int angle) const
    {
        return !m_first_round && speed > HIT_SPEED && angle < CHECK_ANGLE&& m_current_state->next_checkpoint_dist < PREDICT_DISTANCE;
    }

    //
    // Output our drive string
    //
    string GetOutputString()
    {
        Point delta = m_current_state->next_checkpoint - m_current_state->position;

        float x_multiplier = delta.x < 0.0f ? -1 : 1;
        float y_multiplier = delta.y < 0.0f ? -1 : 1;

        int thrust = 100;
        int speed = GetSpeed();

        int angle = abs(m_current_state->next_checkpoint_angle);
        Point goToPoint = m_current_state->next_checkpoint;

        if (m_next_checkpoint_selected || PredictCheckpointHit(speed, angle)) // Can we already start going to the next checkpoint?
        {
            m_next_checkpoint_selected = true;
            goToPoint = GetNextCheckpoint();
            thrust = 20;
        }
        else if (angle > 90) // Big angle? => End thrust so we can turn faster
        {
            thrust = 0;
        }
        else if (m_current_state->next_checkpoint_dist < BREAK_DISTANCE) // Break before the checkpoint
        {
            thrust = lerp(0, 100, (float)abs(m_current_state->next_checkpoint_dist) / BREAK_DISTANCE);
            cerr << "####BREAK distance:" << m_current_state->next_checkpoint_dist << endl;
        }
        else if (angle > CHECK_ANGLE) // Angle getting larger? Slow down
        {
            float abs_angle = (float)angle;
            thrust = lerp(100, 0, abs_angle / 90.0f);
        }
        else // Small angle, not too close => Go go go!!!
        {
            thrust = 100;
        }

        string output;

        //
        // Output our drive commands
        //

        // Boost if we are at the checkpoint with the longest distance between checkpoints and our angle is small enough
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