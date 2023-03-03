<template>
    <div class="flex flex-col justify-center dark:border-white">
        <div>Emom</div>
        <button
            class="mt-4 py-3 px-6 text-xl border-2 rounded-lg active:bg-violet-700"
            @click="$emit('commit', 'clock')"
            >
            Select
        </button>

        <button
            class="mt-4 py-3 px-6 text-xl border-0 rounded-lg active:bg-violet-700"
            @click="$emit('cancel')"
            >
            Cancel
        </button>
    </div>
</template>

<script setup lang="ts">
</script>
