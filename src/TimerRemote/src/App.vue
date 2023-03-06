<template>
  <div class="flex h-full justify-center">
    <div class="min-w-fit w-3/12">
      <StatusControl class="mt-4" :status="status" @pause="sendPause" @start="sendStart"></StatusControl>

      <MainMenu v-if="state === 'menu'" class="mt-4" @select="selectMenu" />

      <SetUpDown v-if="state === 'up' || state == 'down'" :mode="state" @commit="sendCommand" @cancel="cancelSet"></SetUpDown>
      <SetEmom v-if="state === 'emom'" @commit="sendCommand" @cancel="cancelSet"></SetEmom>
    </div>
  </div>
</template>

<script setup lang="ts">
import { onMounted, ref } from 'vue'
import MainMenu from './components/MainMenu.vue'
import SetUpDown from './components/SetUpDown.vue'
import SetEmom from './components/SetEmom.vue'
import StatusControl from './components/StatusControl.vue'

let wsUrl = 'ws://' + window.location.host + '/ws';
if (import.meta.env.MODE !== 'production') {
  wsUrl = 'ws://192.168.4.99/ws'; // for testing
}

const state = ref('menu');
const status = ref({});
let websocket: WebSocket;

function selectMenu(e: string) {
  console.log('select', e);
  state.value = e;
}

function sendCommand(cmd: string) {
  console.log('send command', cmd);
  websocket.send(cmd);
  state.value = 'menu';
}

function sendStart() {
  console.log('send start');
  websocket.send('start;');
  state.value = 'menu';
}

function sendPause() {
  console.log('send pause');
  websocket.send('pause;');
  state.value = 'menu';
}

function cancelSet(cmd: string) {
  state.value = 'menu';
}

function checkWebsocket() {
  if (!websocket || websocket.readyState === WebSocket.CLOSED) {
    startWebsocket();
  }
}

function onOpen(event: Event) {
  console.log('Connection opened');
}

function onClose(event: Event) {
  console.log('Connection closed');
  checkWebsocket();
}

function onMessage(event: MessageEvent) {
  // console.log('message', event);
  status.value = JSON.parse(event.data);
}

function startWebsocket() {
  websocket = new WebSocket(wsUrl);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage;
}

onMounted(() => { 
  startWebsocket();
  setInterval(checkWebsocket, 5000);
});
</script>

<style scoped>
</style>
