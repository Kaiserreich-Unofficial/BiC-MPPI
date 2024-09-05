#include <wmrobot_map.h>
#include <bi_mppi.h>

#include <iostream>
#include <Eigen/Dense>
#include <chrono>

int main() {
    auto model = WMRobotMap();

    using Solver = BiMPPI;
    using SolverParam = BiMPPIParam;
    
    SolverParam param;
    param.dt = 0.1;
    param.Tf = 100;
    param.Tb = 100;
    param.x_init.resize(model.dim_x);
    param.x_init << 2.5, 0.0, M_PI_2;
    param.x_target.resize(model.dim_x);
    param.x_target << 1.5, 5.0, M_PI_2;
    param.Nf = 3000;
    param.Nb = 3000;
    param.Nr = 3000;
    param.gamma_u = 10.0;
    Eigen::VectorXd sigma_u(model.dim_u);
    sigma_u << 0.0, 0.5;
    param.sigma_u = sigma_u.asDiagonal();
    param.deviation_mu = 1.0;
    param.cost_mu = 1.0;
    param.minpts = 5;
    param.epsilon = 0.01;
    param.psi = 1.0;

    int maxiter = 500;
    
    for (int s = 0; s < 3; ++s) {
        switch (s)
        {
        case 0:
            param.x_init(0) = 0.5;
            break;
        case 1:
            param.x_init(0) = 1.5;
            break;
        case 2:
            param.x_init(0) = 2.5;
            break;
        default:
            break;
        }
        // for (int map = 0; map < 300; ++map) {
        for (int map = 299; map >= 0; --map) {
            CollisionChecker collision_checker = CollisionChecker();
            collision_checker.loadMap("../BARN_dataset/txt_files/output_"+std::to_string(map)+".txt", 0.1);
            Solver solver(model);
            solver.U_f0 = Eigen::MatrixXd::Zero(model.dim_u, param.Tf);
            solver.U_b0 = Eigen::MatrixXd::Zero(model.dim_u, param.Tb);
            solver.init(param);
            solver.setCollisionChecker(&collision_checker);
            
            bool is_success = false;
            int i = 0;
            double total_elapsed  = 0.0;
            double f_err = 0.0;
            for (i = 0; i < maxiter; ++i) {
                solver.solve();
                total_elapsed += solver.elapsed;
                
                bool is_collision = false;
                for (int j = 0; j <solver.Xo.cols(); ++j) {
                    if (collision_checker.getCollisionGrid(solver.Xo.col(j))) {
                        is_collision = true;
                        break;
                    }
                }
                if (!is_collision) {
                    f_err = (solver.Xo.rightCols(1) - param.x_target).norm();
                    // if (solver.Xo.rightCols(1)(1) > 4) {
                    if ((solver.Xo.rightCols(1) - param.x_target).norm() < 0.2) {
                        is_success = true;
                        break;
                    }
                }
            }
            std::cout<<s<<'\t'<<map<<'\t'<<is_success<<'\t'<<i<<'\t'<<solver.Xo.cols()<<'\t'<<f_err<<'\t'<<total_elapsed<<std::endl;
            solver.show();
        }
    }    

    return 0;
}
