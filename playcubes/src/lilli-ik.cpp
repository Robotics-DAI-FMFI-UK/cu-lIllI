/// \file lilli-ik.cpp
/// Contains inverse (and forward) kinematics functions using Orocos solver.
#include <stdio.h>
#include <chainiksolverpos_lma.hpp>
#include <chainfksolverpos_recursive.hpp>

#include "lilli-ik.h"

using namespace KDL;
using namespace std;

static int n = 5;
static Chain chain;
static ChainIkSolverPos_LMA *ik_solver;
static ChainFkSolverPos_recursive *fk_solver;

/// print the transformation for the goal position.
void print_frame(Frame pos_goal)
{
        printf("about to reach the point with frame:\n[\n");
        for (int i = 0; i < 4; i++)
          for (int j = 0; j < 4; j++)
          {
            printf("%.2lf", pos_goal(i, j));
            if (j == 3) printf("\n");
            else printf(",\t");
          }
        printf("]\n");
}

/// Initialize the inverse kinematics module.
void lilli_init()
{
        Segment left_shoulder_x = Segment(Joint(Joint::RotX),
                                     Frame(Rotation::RPY(-0.0084844,0.0013988,0),
                                           Vector(0.20, 0.05, -0.07)));

        chain.addSegment(left_shoulder_x);

	Segment left_shoulder_y = Segment(Joint(Joint::RotY),
                                          Frame(Rotation::RPY(-1.5708,-0.0017598,1.5789),
                                                Vector(0.041564, -4.9236E-05, 0)));

        chain.addSegment(left_shoulder_y);

        Segment left_upper_arm = Segment(Joint(Joint::RotZ),
                                          Frame(Rotation::RPY(0.0046444,0,3.1348),
                                                Vector(-0.00034293, 0.003898, -0.040465)));

        chain.addSegment(left_upper_arm);

        Segment left_forearm = Segment(Joint(Joint::RotZ),
                                          Frame(Rotation::RPY(-0.83575,1.5645,-2.4093),
                                                Vector(-0.00073791, -0.00028984, -0.062129)));

        chain.addSegment(left_forearm);

        Segment left_wrist = Segment(Joint(Joint::RotZ),
                                       Frame(Rotation::RPY(-1.5169,1.5629,-1.5149),
                                             Vector(0.13036, 0.00039609, -0.0049841)));

        chain.addSegment(left_wrist);


        Eigen::Matrix<double,6,1> L;
	L(0)=1;L(1)=1;L(2)=1;
	L(3)=0.01;L(4)=0.01;L(5)=0.0; //L(5)=0.01; 

        ik_solver = new ChainIkSolverPos_LMA(chain, L);
	fk_solver = new ChainFkSolverPos_recursive(chain);
}

/// Given the angles of the joints calculate the target position of the tip of the arm.
/// Output is saved to solution[] array: it will contain x, y, z, roll, pitch, yaw (6 valsues). 
int lilli_fk(double *solution, double alpha0, double alpha1, double alpha2, double alpha3, double alpha4)
{
        JntArray q(n);
	q(0) = alpha0;
	q(1) = alpha1;
	q(2) = alpha2;
	q(3) = alpha3;
	q(4) = alpha4;
        Frame pos_goal;
        int retval = fk_solver->JntToCart(q,pos_goal);
	if (retval == 0)
        {
          Vector p = pos_goal.p;
	  Rotation M = pos_goal.M;
	  solution[0] = p(0);
	  solution[1] = p(1);
	  solution[2] = p(2);
	  M.GetRPY(solution[3], solution[4], solution[5]);
	}
	return retval;
}

/// Given the target position x,y,z, roll, pitch, yaw, calculate the angles of the joints.
/// The output is saved to solution[] array - 5 degrees of freedom (5 values)
/// Since the arm has only 5-degrees of freedom, the solution will ignore one of the three input angles.
int lilli_ik(double *solution, double roll, double pitch, double yaw, double x, double y, double z)
{
	JntArray q_init(n);
	JntArray q_sol(n);

        Vector vec(x, y, z);
        Rotation rot = Rotation::RPY(roll, pitch, yaw);
        Frame pos_goal(rot, vec);

        for (int i = 0; i < 5; i++)
          q_init(i) = M_PI / 2;

        print_frame(pos_goal);

	int retval = ik_solver->CartToJnt(q_init,pos_goal,q_sol);

        if (retval == 0)
          for (int i = 0; i < 5; i++)
            solution[i] = q_sol(i);

        return retval;
}

/// Print out the solution containing 5 values for the 5 degrees of freedom (radians).
/// Input is provided as JntArray type of Orocos solver.
void print_solution(JntArray q_sol)
{
    printf("solution=[");
    for (int i = 0; i < 5; i++) 
    {
       printf("%.2lf", q_sol(i));
       if (i < 4) printf(",");
    }
    printf("]\n");
}

/// Print out the solution (target point and angles) in forward-kinematics problem.
void print_fk_solution(double *solution)
{
    printf("solution: x=%.3lf,y=%.3lf,z=%.3lf,r=%.3lf,p=%.2lf,y=%.2lf\n", 
     solution[0],
     solution[1],
     solution[2],
     solution[3],
     solution[4],
     solution[5]);
}

/// Print out the solution containing 5 values for the 5 degrees of freedom (radians).
/// Input is provided as array of doubles as produces by the lilli_ik() function.
void print_ik_solution(double *solution)
{
    printf("solution=[");
    for (int i = 0; i < 5; i++) 
    {
       printf("%.2lf", solution[i]);
       if (i < 4) printf(",");
    }
    printf("]\n");
}

/// Prints 3-dimensional vector (translation element).
void print_frame_vector(Frame pos_goal)
{
        printf("about to reach the point:\n[");
        for (int i = 0; i < 3; i++)
        {
            printf("%.2lf", pos_goal(i, 3));
            if (i == 2) printf("]\n");
            else printf(",\t");
        }
}

// Prints rotation matrix (rotation element).
void print_frame_rotation(Frame pos_goal)
{
        printf("about to reach the point with rotation:\n[\n");
        for (int i = 0; i < 3; i++)
          for (int j = 0; j < 3; j++)
          {
            printf("%.2lf", pos_goal(i, j));
            if (j == 2) printf("\n");
            else printf(",\t");
          }
        printf("]\n");
}

//this was generated by forward kin. solver for angles pi, pi/2, pi/3, pi/4, pi/5
/*
[
-0.25,	0.78,	0.57,	0.23
-0.97,	-0.21,	-0.14,	0.01
0.01,	-0.59,	0.81,	0.25
0.00,	0.00,	0.00,	1.00
]

corresponds to RPY=(-0.63,-0.01,-1.82)

another example:

[
0.14,	0.49,	0.86,	0.35
-0.07,	-0.86,	0.50,	0.16
0.99,	-0.13,	-0.08,	-0.06
0.00,	0.00,	0.00,	1.00
]


corresponds to:

RPY = (-2.12,-1.41,-0.46)

*/

/*
   Rotation rbolo (0.14,	0.49,	0.86,	-0.07,	-0.86,	0.50,	0.99,	-0.13,	-0.08);
   double roll,pitch,yaw;
   rbolo.GetRPY(roll, pitch, yaw);
   printf("rbolo: (%.2lf,%.2lf,%.2lf)\n", roll, pitch, yaw);

*/


