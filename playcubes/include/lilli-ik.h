#ifndef __LILLI_IK__H
#define __LILLI_IK__H

void lilli_init();

int lilli_ik(double *solution, double roll, double pitch, double yaw, double x, double y, double z);
int lilli_fk(double *solution, double alpha0, double alpha1, double alpha2, double alpha3, double alpha4);

void print_ik_solution(double *solution);
void print_fk_solution(double *solution);

#endif

