#include <wmrobot_map.h>
#include <log_mppi.h>

#include <iostream>
#include <Eigen/Dense>
#include <chrono>

int main() {
    auto model = WMRobotMap();

    MPPIParam log_mppi_param;
    log_mppi_param.dt = 0.2;
    log_mppi_param.T = 100;
    log_mppi_param.x_init.resize(model.dim_x);
    log_mppi_param.x_init << 1.5, 0.0, M_PI_2;
    log_mppi_param.x_target.resize(model.dim_x);
    log_mppi_param.x_target << 1.5, 5.0, M_PI_2;
    log_mppi_param.N = 6000;
    log_mppi_param.gamma_u = 10.0;
    Eigen::VectorXd sigma_u(model.dim_u);
    sigma_u << 0.0, 0.5;
    log_mppi_param.sigma_u = sigma_u.asDiagonal();
    
    // for (int i = 299; i >= 0 ; --i) {
    for (int i = 278; i >= 0 ; i) {
    // for (int i = 0; i < 300; ++i) {
        CollisionChecker collision_checker = CollisionChecker();
        collision_checker.loadMap("../BARN_dataset/txt_files/output_"+std::to_string(i)+".txt", 0.1);
        LogMPPI log_mppi(model);
        log_mppi.U_0 = Eigen::MatrixXd::Zero(model.dim_u, log_mppi_param.T);
        log_mppi.init(log_mppi_param);
        log_mppi.setCollisionChecker(&collision_checker);
        
        while (true) {
            log_mppi.solve();
            // std::cout<<"1 solved in "<<log_mppi.elapsed_1.count()<<std::endl;
            // std::cout<<"2 solved in "<<log_mppi.elapsed_2.count()<<std::endl;
            // std::cout<<"3 solved in "<<log_mppi.elapsed_3.count()<<std::endl;
            
            std::cout<<log_mppi.x_init.transpose()<<std::endl;
            if (log_mppi.x_init(1) > 5.0) {break;}
        }
        log_mppi.showTraj();
    }

    return 0;
}
