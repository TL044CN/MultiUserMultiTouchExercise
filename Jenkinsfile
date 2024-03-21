
pipeline {
    agent none
    stages {
        stage('Build') {
            matrix {
                axes {
                    axis {
                        name 'PLATFORM'
                        values /*'windows',*/ 'linux'/*, 'macos'*/
                    }
                    axis {
                        name 'COMPILER'
                        values 'gcc', 'clang'/*, 'msvc'*/
                    }
                    axis {
                        name 'BUILD_TYPE'
                        values 'Release', 'Debug'/*, 'MinSizeRel', 'RelWithDebInfo'*/
                    }
                }

                stages {
                    stage('Select Agent'){
                        agent {
                            label "${PLATFORM}&&${COMPILER}"
                        }
                        stages {
                            stage('Update Dependencies') {
                                steps {
                                    sh '''
                                    apt update -y --fix-missing
                                    apt install -y --fix-missing libgtk-3-dev libopencv-dev
                                    '''
                                }
                            }
                            stage('Download necessary Video File') {
                                steps {
                                    script {
                                        try {
                                            unstash 'video-file'
                                            echo "Stashed video retrieved."
                                        } catch (Exception e) {
                                            echo "Downloading Video File"
                                            sh 'mkdir -p figures'
                                            sh 'curl -o figures/video.avi https://nextcloud.shodan.fyi/s/RyK7teTZ6BqRXYk/download/video.avi'
                                            stash includes: 'figures/video.avi', name: 'video-file'
                                        }
                                    }
                                }
                            }
                            stage('Build') {
                                steps {
                                    script {
                                        try {
                                            echo "Unstashing build folder and opencv"
                                            unstash  'vendor-opencv'
                                            unstash  "build-folder-${PLATFORM}-${COMPILER}"
                                        } catch (Exception e) {
                                            echo "opencv or build folder not stashed"
                                        }
                                        sh """
                                        cmake -B build/ -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
                                        cmake --build build/ --config=${BUILD_TYPE}
                                        """
                                        stash includes: 'vendor/OpenCV/', name: 'vendor-opencv'
                                        stash includes: 'build/', name: "build-folder-${PLATFORM}-${COMPILER}"                                        
                                    }
                                }
                            }
                            stage('Run Tests'){
                                steps {
                                  echo 'Currently no tests.'
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
