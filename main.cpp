#include <bi_wmrobot_map.h>
#include <bi_mppi.h>

#include <omp.h>
#include <EigenRand/EigenRand>

#include <iostream>
#include <Eigen/Dense>
#include <random>
#include <vector>
#include <chrono>

int main() {
    auto model = BiWMRobotMap();

    BiMPPIParam bi_mppi_param;
    bi_mppi_param.x_init.resize(model.dim_x);
    bi_mppi_param.x_init << 1.5, 0.0, M_PI_2;
    bi_mppi_param.x_target.resize(model.dim_x);
    bi_mppi_param.x_target << 1.5, 5.0, M_PI_2;
    bi_mppi_param.Nf = 300;
    bi_mppi_param.Nb = 300;
    bi_mppi_param.gamma_u = 10.0;
    Eigen::VectorXd sigma_u(model.dim_u);
    sigma_u << 0.0, 0.9;
    bi_mppi_param.sigma_u = sigma_u.asDiagonal();
    bi_mppi_param.deviation_mu = 1.0;
    bi_mppi_param.cost_mu = 1.0;
    bi_mppi_param.minpts = 4;
    bi_mppi_param.epsilon = 0.02;

    for (int i = 0; i < 300; ++i) {
        CollisionChecker collision_checker = CollisionChecker();
        collision_checker.loadMap("../BARN_dataset/txt_files/output_"+std::to_string(i)+".txt", 0.1);

        BiMPPI bi_mppi(model);
        bi_mppi.init(bi_mppi_param);
        bi_mppi.setCollisionChecker(&collision_checker);

        auto start = std::chrono::high_resolution_clock::now();
        bi_mppi.solve();
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout<<"solved in "<<elapsed.count()<<std::endl;
        
        bi_mppi.show();
    }

    return 0;
}
