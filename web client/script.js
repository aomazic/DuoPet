document.addEventListener('DOMContentLoaded', (event) => {
    const feedButton = document.getElementById('feedButton');
    const feedAmount = document.getElementById('feedAmount');
    const logEntries = document.getElementById('logEntries');
    const connectionStatus = document.getElementById('connectionStatus');
  
    const ws = new WebSocket('ws://192.168.X.XXX:8888/ws');
  
    ws.onopen = function () {
      connectionStatus.textContent = 'Connected';
    };
  
    ws.onclose = function () {
      connectionStatus.textContent = 'Disconnected';
    };
  
    ws.onmessage = function (event) {
      const message = document.createElement('div');
      message.textContent = event.data;
      logEntries.appendChild(message);
    };
  
    feedButton.addEventListener('click', function () {
      const amount = feedAmount.value;
      if (amount) {
        ws.send(JSON.stringify({ action: 'feed', amount }));
        feedAmount.value = '';
      } else {
        alert('Please enter the amount of food.');
      }
    });
  });
  