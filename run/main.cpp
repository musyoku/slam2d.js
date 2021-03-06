#include "include.h"
#include <iostream>
#include <memory>
#include <stdio.h>
#include <vector>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void build_field(slam::environment::Field* field)
{
    float wall_thickness = 0.05;
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, -1);
        wall.emplace_back(-1, -1 + wall_thickness);
        wall.emplace_back(1, -1);
        wall.emplace_back(1, -1 + wall_thickness);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1 + wall_thickness, -1);
        wall.emplace_back(-1, -1);
        wall.emplace_back(-1 + wall_thickness, 1);
        wall.emplace_back(-1, 1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, 1 - wall_thickness);
        wall.emplace_back(-1, 1);
        wall.emplace_back(1, 1 - wall_thickness);
        wall.emplace_back(1, 1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(1, -1);
        wall.emplace_back(1 - wall_thickness, -1);
        wall.emplace_back(1, 1);
        wall.emplace_back(1 - wall_thickness, 1);
        field->add_wall(wall);
    }

    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.2, 0.1);
        wall.emplace_back(-0.2, -0.1);
        wall.emplace_back(0.1, 0.1);
        wall.emplace_back(0.1, -0.1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.2, 0.2);
        wall.emplace_back(-0.2, -0.1);
        wall.emplace_back(-0.1, 0.2);
        wall.emplace_back(-0.1, -0.1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.1, 0);
        wall.emplace_back(-0.1, -0.2);
        wall.emplace_back(0.1, 0);
        wall.emplace_back(0.1, -0.2);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(0, 0.1);
        wall.emplace_back(0.3, 0.1);
        wall.emplace_back(0, -0.1);
        wall.emplace_back(0.3, -0.1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(0, 0.3);
        wall.emplace_back(0.2, 0.3);
        wall.emplace_back(0, 0);
        wall.emplace_back(0.2, 0);
        field->add_wall(wall);
    }

    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, -0.9);
        wall.emplace_back(0.3, -0.9);
        wall.emplace_back(-1, -1);
        wall.emplace_back(0.3, -1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, -0.8);
        wall.emplace_back(-0.3, -0.8);
        wall.emplace_back(-1, -1);
        wall.emplace_back(-0.3, -1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.1, 1);
        wall.emplace_back(0.5, 1);
        wall.emplace_back(-0.1, 0.9);
        wall.emplace_back(0.5, 0.9);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, 1);
        wall.emplace_back(-0.7, 1);
        wall.emplace_back(-1, 0.7);
        wall.emplace_back(-0.7, 0.7);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(1, -1);
        wall.emplace_back(0.7, -1);
        wall.emplace_back(1, -0.7);
        wall.emplace_back(0.7, -0.7);
        field->add_wall(wall);
    }
}

int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!!glfwInit() == false) {
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1920, 800, "slam2d", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    glm::vec4 bg_color = glm::vec4(61.0f / 255.0f, 57.0f / 255.0f, 90.0f / 255.0f, 1.00f);

    std::unique_ptr<slam::environment::Field> field = std::make_unique<slam::environment::Field>();
    build_field(field.get());

    std::unique_ptr<glgui::view::Field> fieldView = std::make_unique<glgui::view::Field>(field.get());
    std::unique_ptr<glgui::view::Observer> observerView = std::make_unique<glgui::view::Observer>(bg_color);
    std::unique_ptr<glgui::view::Robot> robotView = std::make_unique<glgui::view::Robot>(0.1);
    std::unique_ptr<glgui::view::Trajectory> actualTrajectoryView = std::make_unique<glgui::view::Trajectory>(glm::vec3(153.0f / 255.0f, 214.0f / 255.0f, 202.0f / 255.0f));
    std::unique_ptr<glgui::view::Trajectory> estimatedTrajectoryView = std::make_unique<glgui::view::Trajectory>(glm::vec3(128.0f / 255.0f, 99.0f / 255.0f, 187.0f / 255.0f));
    std::unique_ptr<glgui::view::Map> mapView = std::make_unique<glgui::view::Map>();

    std::unique_ptr<glgui::component::Parameters> paramComponent = std::make_unique<glgui::component::Parameters>(50, 0.5, 10);
    std::unique_ptr<glgui::component::Noise> noiseComponent = std::make_unique<glgui::component::Noise>(0, 0);
    std::unique_ptr<glgui::component::Method> methodComponent = std::make_unique<glgui::component::Method>();
    std::unique_ptr<glgui::frame::Main> mainFrame = std::make_unique<glgui::frame::Main>(paramComponent.get(), methodComponent.get(), noiseComponent.get());

    std::unique_ptr<slam::lidar::Observer> observer = std::make_unique<slam::lidar::Observer>(noiseComponent->_scan_stddev);
    std::unique_ptr<slam::scan_matching::Associator> associator = std::make_unique<slam::scan_matching::Associator>(0.2);
    std::unique_ptr<slam::scan_matching::IterativeClosestPoints> icp = std::make_unique<slam::scan_matching::IterativeClosestPoints>(associator.get(), 0.001, 1000, 0.01);

    int time_step = 0;
    std::vector<GLfloat> actual_trajectory; // 実際の軌跡
    std::vector<GLfloat> estimated_trajectory; // 予測した軌跡
    std::vector<GLfloat> map; // 生成された地図
    std::vector<glm::vec4> scans; // レーザースキャナから得られた値

    double round_angle_rad = M_PI_2;
    double robot_rotation_noise = 0;
    glm::vec2 last_scan_location;
    double last_scan_robot_angle_rad;
    bool is_first_scan = false;

    // ロボットが環境内を回転移動するときの1ステップの角度
    double round_base_angle_rad = M_PI / 100.0f * paramComponent->_speed;
    // ロボットが回転移動するときの半径
    double moving_radius = 0.65f;

    // ロボット自身が予測したロボットの現在位置
    glm::vec2 estimated_location;
    // 実際のロボットの位置
    glm::vec2 actual_location;

    // 前回のスキャン
    std::vector<glm::vec2> prev_scan_points;

    while (!!glfwWindowShouldClose(window) == false) {
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        int screen_width, screen_height;
        glfwGetFramebufferSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);
        glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        unsigned int square_length = screen_width / 3.0;

        if (mainFrame->_is_slam_running) {
            if (is_first_scan) {
                time_step = 0;
                map.clear();
                actual_trajectory.clear();
                estimated_trajectory.clear();
                round_angle_rad = M_PI_2;
            }
            observer->_noise_stddev = noiseComponent->_scan_stddev;

            int num_beams = paramComponent->_num_beams;
            double cursor_x, cursor_y;
            glfwGetCursorPos(window, &cursor_x, &cursor_y);
            round_angle_rad -= round_base_angle_rad;

            // オドメトリによる位置予測
            if (is_first_scan) {
                estimated_location.x = moving_radius * cos(round_angle_rad);
                estimated_location.y = moving_radius * sin(round_angle_rad);
                estimated_trajectory.emplace_back(estimated_location.x);
                estimated_trajectory.emplace_back(estimated_location.y);
                actual_location.x = estimated_location.x;
                actual_location.y = estimated_location.y;
                actual_trajectory.emplace_back(actual_location.x);
                actual_trajectory.emplace_back(actual_location.y);
            } else {
                // 微小時間ではロボットは直進しているとみなす
                double delta_distance = sin(round_base_angle_rad / 2.0) * moving_radius * 2.0;
                double delta_angle_rad = -(M_PI_2 - (M_PI - round_base_angle_rad) / 2.0);
                double delta_moving_distance_x = cos(delta_angle_rad) * delta_distance;
                double delta_moving_distance_y = sin(delta_angle_rad) * delta_distance;
                double transform_angle_rad = -(M_PI_2 - round_angle_rad);

                double prev_location_x = estimated_trajectory[estimated_trajectory.size() - 2];
                double prev_location_y = estimated_trajectory[estimated_trajectory.size() - 1];
                estimated_location.x = cos(transform_angle_rad) * delta_moving_distance_x - sin(transform_angle_rad) * delta_moving_distance_y + prev_location_x;
                estimated_location.y = sin(transform_angle_rad) * delta_moving_distance_x + cos(transform_angle_rad) * delta_moving_distance_y + prev_location_y;

                // 実際のロボット位置にはノイズが乗る
                if (noiseComponent->_odometry_stddev > 0) {
                    double prev_location_x = actual_trajectory[actual_trajectory.size() - 2];
                    double prev_location_y = actual_trajectory[actual_trajectory.size() - 1];
                    if (time_step % 5 == 0) { // あまり高頻度で乗せるとリアルではなくなりそう
                        robot_rotation_noise = M_PI * slam::sampler::normal(0, noiseComponent->_odometry_stddev);
                    }
                    double transform_angle_rad = -(M_PI_2 - round_angle_rad) + robot_rotation_noise;
                    actual_location.x = cos(transform_angle_rad) * delta_moving_distance_x - sin(transform_angle_rad) * delta_moving_distance_y + prev_location_x;
                    actual_location.y = sin(transform_angle_rad) * delta_moving_distance_x + cos(transform_angle_rad) * delta_moving_distance_y + prev_location_y;
                } else {
                    actual_location.x = estimated_location.x;
                    actual_location.y = estimated_location.y;
                }

                actual_trajectory.emplace_back(actual_location.x);
                actual_trajectory.emplace_back(actual_location.y);
                estimated_trajectory.emplace_back(estimated_location.x);
                estimated_trajectory.emplace_back(estimated_location.y);

                // レーザースキャナによる観測
                if (time_step % paramComponent->_laser_scanner_interval == 0) {
                    if (scans.size() != paramComponent->_num_beams) {
                        scans.resize(paramComponent->_num_beams);
                    }
                    for (int n = 0; n < num_beams; n++) {
                        scans[n] = glm::vec4(0, 0, 0, 0);
                    }
                    double estimated_robot_angle_rad = round_angle_rad - M_PI_2;
                    double actual_robot_angle_rad = estimated_robot_angle_rad + robot_rotation_noise;

                    observer->observe(field.get(), actual_location, actual_robot_angle_rad, num_beams, scans);
                    last_scan_location.x = actual_location.x;
                    last_scan_location.y = actual_location.y;
                    last_scan_robot_angle_rad = actual_robot_angle_rad;

                    // 地図構築
                    // スキャンマッチング
                    std::vector<glm::vec2> current_scan_points;
                    for (int n = 0; n < num_beams; n++) {
                        // スキャンデータをセンサ座標系に変換
                        glm::vec4& scan = scans[n];
                        double distance = scan[2];
                        double angle_rad = scan[3];
                        glm::vec2 point;
                        point.x = cos(angle_rad) * distance;
                        point.y = sin(angle_rad) * distance;
                        // if (methodComponent->_odometry_enabled) {
                        //     // 座標系を変換
                        //     angle_rad += estimated_robot_angle_rad;
                        //     // ロボットはノイズの影響がわからないので予測位置を使う
                        //     point.x = cos(angle_rad) * distance + estimated_location.x;
                        //     point.y = sin(angle_rad) * distance + estimated_location.y;
                        // } else {
                        //     point.x = cos(angle_rad) * distance;
                        //     point.y = sin(angle_rad) * distance;
                        // }
                        // map.emplace_back(point.x);
                        // map.emplace_back(point.y);
                        current_scan_points.emplace_back(point);
                    }

                    if (methodComponent->_scan_matching_enabled && prev_scan_points.size() == current_scan_points.size()) {
                        glm::vec2 d0(delta_moving_distance_x, delta_moving_distance_y);
                        double estimated_delta_algne_rad = delta_angle_rad;
                        icp->match(prev_scan_points, current_scan_points, d0, estimated_delta_algne_rad);
                        std::cout << delta_moving_distance_x << ", " << delta_moving_distance_y << " -> " << d0.x << ", " << d0.y << std::endl;

                        double transform_angle_rad = -(M_PI_2 - round_angle_rad);
                        double prev_location_x = estimated_trajectory[estimated_trajectory.size() - 2];
                        double prev_location_y = estimated_trajectory[estimated_trajectory.size() - 1];
                        estimated_location.x = cos(transform_angle_rad) * d0.x - sin(transform_angle_rad) * d0.y + prev_location_x;
                        estimated_location.y = sin(transform_angle_rad) * d0.x + cos(transform_angle_rad) * d0.y + prev_location_y;

                        for (int n = 0; n < num_beams; n++) {
                            // スキャンデータをセンサ座標系に変換
                            glm::vec4& scan = scans[n];
                            double distance = scan[2];
                            double angle_rad = scan[3];
                            glm::vec2 point;
                            point.x = cos(angle_rad) * distance;
                            point.y = sin(angle_rad) * distance;
                            if (methodComponent->_odometry_enabled) {
                                // 座標系を変換
                                angle_rad += estimated_robot_angle_rad;
                                // ロボットはノイズの影響がわからないので予測位置を使う
                                point.x = cos(angle_rad) * distance + estimated_location.x;
                                point.y = sin(angle_rad) * distance + estimated_location.y;
                            } else {
                                point.x = cos(angle_rad) * distance;
                                point.y = sin(angle_rad) * distance;
                            }
                            map.emplace_back(point.x);
                            map.emplace_back(point.y);
                        }
                    } else {
                        for (int n = 0; n < num_beams; n++) {
                            // スキャンデータをセンサ座標系に変換
                            glm::vec4& scan = scans[n];
                            double distance = scan[2];
                            double angle_rad = scan[3];
                            glm::vec2 point;
                            point.x = cos(angle_rad) * distance;
                            point.y = sin(angle_rad) * distance;
                            if (methodComponent->_odometry_enabled) {
                                // 座標系を変換
                                angle_rad += estimated_robot_angle_rad;
                                // ロボットはノイズの影響がわからないので予測位置を使う
                                point.x = cos(angle_rad) * distance + estimated_location.x;
                                point.y = sin(angle_rad) * distance + estimated_location.y;
                            } else {
                                point.x = cos(angle_rad) * distance;
                                point.y = sin(angle_rad) * distance;
                            }
                            map.emplace_back(point.x);
                            map.emplace_back(point.y);
                        }
                    }
                    prev_scan_points = current_scan_points;
                }
            }

            time_step++;
            if (is_first_scan) {
                is_first_scan = false;
            }

        } else {
            is_first_scan = true;
            actual_location.x = moving_radius * cos(round_angle_rad);
            actual_location.y = moving_radius * sin(round_angle_rad);
            last_scan_location.x = actual_location.x;
            last_scan_location.y = actual_location.y;
        }

        fieldView->render(window, 0, 0, square_length, square_length);
        mapView->render(window, square_length * 2, 0, square_length, square_length, map);
        observerView->render(window, square_length, 0, square_length, square_length, last_scan_location, scans);
        if (methodComponent->_odometry_enabled) {
            estimatedTrajectoryView->render(window, square_length * 2, 0, square_length, square_length, estimated_trajectory);
            actualTrajectoryView->render(window, square_length * 2, 0, square_length, square_length, actual_trajectory);
        }

        double robot_angle_rad = round_angle_rad - M_PI_2 + robot_rotation_noise;
        robotView->render(window, 0, 0, square_length, square_length, actual_location, robot_angle_rad);
        robotView->render(window, square_length, 0, square_length, square_length, last_scan_location, last_scan_robot_angle_rad);

        mainFrame->render();

        glViewport(0, 0, screen_width, screen_height);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
