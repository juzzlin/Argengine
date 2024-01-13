pipeline {
    agent any
    stages {
        stage('CMake Debug build and unit tests / Ubuntu 18.04') {
            agent {
                docker {
                    image 'juzzlin/qt5-18.04:latest'
                    args '--privileged -t -v $WORKSPACE:/Argengine'
                }
            }
            steps {
                sh "mkdir -p build-debug"
                sh "cd build-debug && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug .."
                sh "cd build-debug && cmake --build . && ctest"
            }
        }
        stage('CMake Release build and unit tests / Ubuntu 18.04') {
            agent {
                docker {
                    image 'juzzlin/qt5-18.04:latest'
                    args '--privileged -t -v $WORKSPACE:/Argengine'
                }
            }
            steps {
                sh "mkdir -p build-release"
                sh "cd build-release && cmake -GNinja -DCMAKE_BUILD_TYPE=Release .."
                sh "cd build-release && cmake --build . && ctest"
            }
        }
    }
}

