#ifndef OPENPOSE_EXAMPLES_TUTORIAL_USER_POST_PROCESSING_HPP
#define OPENPOSE_EXAMPLES_TUTORIAL_USER_POST_PROCESSING_HPP

#include <openpose/core/common.hpp>
#include <openpose/core/cvMatToOpInput.hpp>
#include "userDatum.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace op
{
    /**
     * Add your class description here.
     */

    class UserPostProcessing
    {
        std::string pipe_name;
        int fd;
        std::string roll_text;
        int font_face;
        double font_scale;
        int thickness;
        int text_offset;


    public:
        /**
         * Add your constructor description here.
         */
        UserPostProcessing();


        /**
         * Add your initializationOnThread description here.
         * This is equivalent to the constructor, but it is run in the thread where this function will operate
         */
        void initializationOnThread();

        /**
         * Add your information about what your class does and their inputs/outputs.
         * @param output is the modified cv::Mat.
         * @param input is the input cv::Mat.
         * @return If it is not void, add returning information here.
         */
        void doSomething(op::UserDatum& datam);

    private:
        /**
         * Description of your variable here.
         */

        void addText(cv::Mat &image, int num_people) {
            int baseline = 0;
            cv::Size text_size = cv::getTextSize(roll_text, font_face, font_scale, thickness, &baseline);
            baseline += thickness;
            cv::Mat base_image = cv::Mat::zeros(cv::Size(text_size.width+2*image.cols+10, text_size.height+10), CV_8UC3);
            cv::Point text_org(image.cols, text_size.height);

            image(cv::Rect(0, 0, image.cols, base_image.rows)).copyTo(
                    base_image(cv::Rect(text_offset, 0, image.cols, base_image.rows)));
            cv::putText(base_image, roll_text, text_org, font_face, font_scale, cv::Scalar::all(255), thickness, 8);
            base_image(cv::Rect(text_offset, 0, image.cols, base_image.rows)).copyTo(
                    image(cv::Rect(0, 0, image.cols, base_image.rows)));

            text_offset += 5;
            if (text_offset > text_size.width+image.cols) {
                text_offset = 0;
            }

            std::string people_text = "People: " + std::to_string(num_people);
            text_org = cv::Point(0, image.rows-20);
            cv::putText(image, people_text, text_org, font_face, font_scale, cv::Scalar::all(255), thickness, 8);
        }
        // bool mSomeVariableHere;
    };
}





// Implementation
namespace op
{
    UserPostProcessing::UserPostProcessing(): pipe_name("/tmp/mypipe.fifo"),
                                              font_face(cv::FONT_HERSHEY_COMPLEX),
                                              font_scale(1), thickness(2), text_offset(0)
    {
        try
        {
            fd = open(pipe_name.c_str(), O_WRONLY);
            roll_text = "This camera is collecting information for the Aflex team. The footage captured by "
                        "this camera is analyzed and filtered before finally sending the number of people "
                        "that were present. The Aflex team will be the only group to access this data to "
                        "determine if crosswalk light timing can be improved. The data collected from this "
                        "camera will be held onto for 30 days by the team and then deleted. Thank you for "
                        "helping us improve the city!";
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    void UserPostProcessing::initializationOnThread()
    {
        try
        {
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    void UserPostProcessing::doSomething(op::UserDatum& datam)
    {
        try
        {
            // Random operation on data
            // cv::bitwise_not(datam.cvOutputData, datam.cvOutputData);

            const auto &poseKeypoints = datam.poseKeypoints;
            for (auto person = 0; person < poseKeypoints.getSize(0); person++) {

                // We only care about nose, ear, eye in the pose rendering
                const std::vector<int> related_index = {0, 14, 15, 16, 17};
                const std::vector<double> scale_list = {0, 15, 5, 3, 1.5, 1};

                std::vector<double> x_list(0), y_list(0);
                for (auto it = related_index.begin(); it != related_index.end(); it++) {
                    auto x = poseKeypoints[{person, *it, 0}];
                    auto y = poseKeypoints[{person, *it, 1}];
                    if (x != 0 || y != 0) {
                        x_list.push_back(x);
                        y_list.push_back(y);
                    }
                }

                if (x_list.size() > 1) {
                    auto min_x = *std::min_element(x_list.begin(), x_list.end());
                    auto max_x = *std::max_element(x_list.begin(), x_list.end());
                    auto min_y = *std::min_element(y_list.begin(), y_list.end());
                    auto max_y = *std::max_element(y_list.begin(), y_list.end());

                    auto &image = datam.cvOutputData;

                    if (max_x-min_x > max_y-min_y) {
                        // extend y
                        auto diff = (max_x-min_x) - (max_y-min_y);
                        max_y += diff / 2;
                        min_y -= diff / 2;
                    } else {
                        // extend x
                        auto diff = (max_y-min_y) - (max_x-min_x);
                        max_x += diff / 2;
                        min_x -= diff / 2;
                    }

                    auto size = (max_x-min_x);
                    max_x += size*((scale_list[x_list.size()]-1)/2);
                    min_x -= size*((scale_list[x_list.size()]-1)/2);
                    max_y += size*((scale_list[x_list.size()]-1)/2);
                    min_y -= size*((scale_list[x_list.size()]-1)/2);

                    max_x = max_x > image.cols ? image.cols : max_x;
                    min_x = min_x < 0 ? 0 : min_x;
                    max_y = max_y > image.rows ? image.rows : max_y;
                    min_y = min_y < 0 ? 0 : min_y;

                    // std::cout<<min_x<<" "<<max_x<<" "<<min_y<<" "<<max_y<<std::endl;

                    cv::Rect region(min_x, min_y, max_x-min_x, max_y-min_y);
                    cv::GaussianBlur(datam.cvOutputData(region), datam.cvOutputData(region), cv::Size(0,0), 25);
                }
            }

            int32_t num_people = poseKeypoints.getSize(0);
            addText(datam.cvOutputData, num_people);
            write(fd, &num_people, sizeof(num_people));

        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    // COMPILE_TEMPLATE_DATUM(UserPostProcessing);
}

#endif // OPENPOSE_EXAMPLES_TUTORIAL_USER_POST_PROCESSING_HPP
