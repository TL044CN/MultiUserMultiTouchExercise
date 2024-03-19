
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
                            stage('Prepare') {
                                steps {
                                    sh '''
                                    apt install -y libgtk-3-dev
                                    '''
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
                        }
                    }
                }
            }
        }
    }
}