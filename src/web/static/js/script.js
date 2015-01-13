var app = angular.module('app', []);
var ws = new WebSocket(WS);

app.factory('ChatService', function() {
    var service = {};

    service.connect = function() {

        if(service.ws) { return; }

        ws.onopen = function() {
            console.info("Succeeded to open a connection");
        };

        ws.onerror = function() {
            alert("Failed to open a connection");
        }

        ws.onmessage = function(message) {
            service.callback(message.data);
            console.info("Recieve data:" + message.data);
        };

        service.ws = ws;
    }

    service.send = function() {
        ws.send("");
    }

    service.subscribe = function(callback) {
        service.callback = callback;
    }

    return service;
});

function MyCtrl($scope, ChatService) {
    $scope.messages = [];
    ChatService.connect();

    ChatService.subscribe(function(data) {
        var obj = angular.fromJson(data);
        $scope.messages = obj.rows;
        $scope.lastUpdate = obj.lastUpdate;

        for (i = 0; i < obj.rows.length; i++) {
            if (obj.rows[i].value > 0) {
                $scope.messages[i].isChecked = true;
            } else {
                $scope.messages[i].isChecked = false;
            }
        }

        //console.info(obj.rows);
        $scope.$apply(obj.rows);
    });

    $scope.setCheckbox = function(id) {
        for (i = 0; i < $scope.messages.length; i++) {
            if ($scope.messages[i].id == id) {
                if ($scope.messages[i].value == 0 && $scope.messages[i].isChecked) {
                    $scope.messages[i].value = 255;
                } else {
                    $scope.messages[i].value = 0;
                }
                ws.send(angular.toJson($scope.messages[i]));
                break;
            }
        }
    };

    $scope.updateSlider = function(id) {
        for (i = 0; i < $scope.messages.length; i++) {
            if ($scope.messages[i].id == id) {
                if ($scope.messages[i].value > 0 && !$scope.messages[i].isChecked) {
                    $scope.messages[i].isChecked = true;
                } else {
                    $scope.messages[i].isChecked = false;
                }
                ws.send(angular.toJson($scope.messages[i]));
                break;
            }
        }
    };

}

setInterval(function () { ws.send("")}, 2000);
