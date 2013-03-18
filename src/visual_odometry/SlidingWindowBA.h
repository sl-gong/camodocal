#ifndef SLIDINGWINDOWBA_H
#define SLIDINGWINDOWBA_H

#include <Eigen/Dense>
#include <list>

#include "../catacamera/CataCamera.h"
#include "../sparse_graph/SparseGraph.h"

namespace camodocal
{

class SlidingWindowBA
{
public:
    enum
    {
        ODOMETER = 0,
        VO = 1
    };

    SlidingWindowBA(const CataCamera::Parameters& cameraParameters,
                    int N = 10, int n = 3, int mode = VO,
                    Eigen::Matrix4d globalCameraPose = Eigen::Matrix4d());

    Eigen::Matrix4d globalCameraPose(void);

    bool addFrame(FramePtr& frame,
                  const Eigen::Matrix3d& R_rel, const Eigen::Vector3d& t_rel,
                  Eigen::Matrix3d& R, Eigen::Vector3d& t);

    void clear(void);
    bool empty(void) const;
    size_t windowSize(void) const;

    void setVerbose(bool verbose);

    int N(void);
    int n(void);

    FramePtr& currentFrame(void);
    std::vector<Eigen::Matrix4d, Eigen::aligned_allocator<Eigen::Matrix4d> > poses(void) const;
    std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > scenePoints(void) const;

    void frameReprojectionError(int windowIdx, double& minError, double& maxError, double& avgError) const;
    void windowReprojectionError(double& minError, double& maxError, double& avgError) const;

private:
    void findFeatureCorrespondences(const std::vector<Point2DFeaturePtr>& features,
                                    int nViews,
                                    std::vector<std::vector<Point2DFeaturePtr> >& correspondences) const;

    bool project3DPoint(const Eigen::Quaterniond& q, const Eigen::Vector3d& t,
                        const Eigen::Vector4d& src, Eigen::Vector3d& dst) const;

    void rectifyImagePoint(const cv::Point2f& src, cv::Point2f& dst, double focal = 1.0) const;
    void rectifyImagePoint(const Eigen::Vector2d& src, Eigen::Vector2d& dst, double focal = 1.0) const;

    void rectifyImagePoints(const std::vector<cv::Point2f>& src,
                            std::vector<cv::Point2f>& dst,
                            double focal = 1.0) const;

    void triangulatePoints(const Eigen::Quaterniond& q1,
                           const Eigen::Vector3d& t1,
                           const std::vector<cv::Point2f>& imagePoints1,
                           const Eigen::Quaterniond& q2,
                           const Eigen::Vector3d& t2,
                           const std::vector<cv::Point2f>& imagePoints2,
                           std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> >& points3D,
                           std::vector<size_t>& inliers) const;

    void tvt(const Eigen::Quaterniond& q1,
             const Eigen::Vector3d& t1,
             const std::vector<cv::Point2f>& imagePoints1,
             const Eigen::Quaterniond& q2,
             const Eigen::Vector3d& t2,
             const std::vector<cv::Point2f>& imagePoints2,
             const Eigen::Quaterniond& q3,
             const Eigen::Vector3d& t3,
             const std::vector<cv::Point2f>& imagePoints3,
             std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> >& points3D,
             std::vector<size_t>& inliers) const;

    void optimize(void);

    int m_N;
    int m_n;
    int mMode;

    Pose m_T_cam_odo;

    std::list<FramePtr> mWindow;

    CataCamera mCamera;

    const double kMinDisparity;
    const double kNominalFocalLength;
    const double kReprojErrorThresh;
    const double kTVTReprojErrorThresh;

    size_t mFrameCount;
    bool mVerbose;

    const int kMin2D2DFeatureCorrespondences;
    const int kMin2D3DFeatureCorrespondences;
};

}

#endif
