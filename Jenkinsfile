
pipeline {
    agent none
    stages {
        stage('initialize submodules') {
            agent any
            steps {
                script {
                    sh 'git submodule update --init --recursive'
                    if (fileExists('vendor/OpenCV')) {
                        stash name: "vendor", includes: 'vendor/OpenCV/**'
                    } else {
                        echo 'vendor/OpenCV directory not found.'
                        currentBuild.result = 'UNSTABLE'
                    }
                }
            }
        }
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
                                    apt install -y --fix-missing libgtk-3-dev libopencv-dev
                                    '''
                                }
                            }
                            stage('Retrieving Artifacts') {
                                steps {
                                    script{
                                        try {
                                            unstash 'vendor'
                                        } catch(Exception e) {

                                        }
                                        try {
                                            unarchive (mapping: [
                                                'figures/video.avi': 'figures/video.avi',
                                                'vendor/OpenCV': 'vendor/OpenCV',
                                                "build-${PLATFORM}-${COMPILER}/": "build"
                                            ])
                                            artifactsRetrieved = true
                                        } catch (Exception e) {
                                            sh 'mkdir -p figures'
                                            sh 'curl -o figures/video.avi https://nextcloud.shodan.fyi/s/RyK7teTZ6BqRXYk/download/video.avi'
                                        }
                                    }
                                }
                            }
                            stage('Build') {
                                steps {
                                    sh """
                                    cmake -B build/ -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
                                    cmake --build build/ --config=${BUILD_TYPE}
                                    """
                                }
                            }
                            stage('Run Tests'){
                                steps {
                                  echo 'Currently no tests.'
                                }
                            }
                            stage('Archiving Artifacts') {
                                steps {
                                    sh 'mv build "build-${PLATFORM}-${COMPILER}"'
                                    archiveArtifacts (
                                        artifacts: [
                                            'figures/video.avi',
                                            'vendor/OpenCV/',
                                            "build-${PLATFORM}-${COMPILER}/"
                                        ],
                                        allowEmptyArchive: true, onlyIfSuccessful: true, fingerprint: true
                                    )
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
