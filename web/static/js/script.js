var app = angular.module('app', []);

app.factory('socket', function ($rootScope) {
  var socket = io.connect(wsUrl);
  return {
    on: function (eventName, callback) {
      socket.on(eventName, function () {
        var args = arguments;
        $rootScope.$apply(function () {
          callback.apply(socket, args);
        });
      });
    },
    emit: function (eventName, data, callback) {
      socket.emit(eventName, data, function () {
        var args = arguments;
        $rootScope.$apply(function () {
          if (callback) {
            callback.apply(socket, args);
          }
        });
      })
    }
  };
});

function MyCtrl($scope, socket) {

    $scope.messages = [];

    socket.on('init', function(data) {
        var obj = angular.fromJson(data);
        if (obj.action == "load") {
            $scope.messages = obj.rows;
            $scope.lastUpdate = obj.lastUpdate;

            for (i = 0; i < obj.rows.length; i++) {
                if (obj.rows[i].value > 0) {
                    $scope.messages[i].isChecked = true;
                } else {
                    $scope.messages[i].isChecked = false;
                }
            }
        }
    });

    socket.on('recv:updateAll', function(data) {
        var obj = angular.fromJson(data.data.rows);
        for (i = 0; i < $scope.messages.length; i++) {
            if ($scope.messages[i].id == obj[i].id) {
                $scope.messages[i].value = obj[i].value;
            }
        }
    });

    socket.on('recv:updateOneSlider', function(data) {
        var obj = angular.fromJson(data.data);
        for (i = 0; i < $scope.messages.length; i++) {
            if ($scope.messages[i].id == obj.id) {
                $scope.messages[i].value = obj.value;
                break;
            }
        }
    });

    $scope.setCheckbox = function(id) {
        for (i = 0; i < $scope.messages.length; i++) {
            if ($scope.messages[i].id == id) {
                if ($scope.messages[i].value == 0 && $scope.messages[i].isChecked) {
                    $scope.messages[i].value = 16;
                } else {
                    $scope.messages[i].value = 0;
                }
                $scope.sendMessage(angular.toJson($scope.messages[i]));
                break;
            }
        }
    };

    $scope.sendMessage = function (data) {
        socket.emit('send:updateSlider', {
          message: data
        });
    };

    $scope.updateSlider = function(id) {
        for (i = 0; i < $scope.messages.length; i++) {
            if ($scope.messages[i].id == id) {
                if ($scope.messages[i].value > 0) {
                    $scope.messages[i].isChecked = true;
                } else {
                    $scope.messages[i].isChecked = false;
                }
                //console.info("Sending data ws:" + angular.toJson($scope.messages[i]));
                $scope.sendMessage(angular.toJson($scope.messages[i]));
                break;
            }
        }
    };
/*
    setInterval(function () {
        scope.$apply(function () {
            scope.date = getDate();
    });*/
});
}

//setInterval(function () { ws.send("")}, 2000);
