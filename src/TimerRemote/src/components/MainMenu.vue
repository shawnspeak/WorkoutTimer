<template>
    <div class="flex flex-col justify-center dark:border-white">
      <!-- <button
            class="mt-4 py-3 px-6 text-xl border-2 rounded-lg active:bg-violet-700"
            @click="$emit('select', 'clock')"
            >Clock</button> -->
        <button
            class="mt-4 py-3 px-6 text-xl border-2 rounded-lg active:bg-violet-700"
            @click="$emit('select', 'down')"
            >For time</button>
        <button
            class="mt-4 py-3 px-6 text-xl border-2 rounded-lg active:bg-violet-700"
            @click="$emit('select', 'emom')"
            >EMOM</button>
        <button
            class="mt-4 py-3 px-6 text-xl border-2 rounded-lg active:bg-violet-700"
            @click="$emit('select', 'up')"
            >AMRAP</button>
    </div>
</template>

<script setup lang="ts">
</script>
