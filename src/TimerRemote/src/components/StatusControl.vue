<template>
    <div class="dark:border-white border-2">
        <div>Mode: {{ mode() }}</div>
        <div>Status: {{ status() }}</div>
        <div>Time: {{ time() }}</div>

        <button
            v-if="canStart()"
            class="mt-4 py-3 px-6 text-xl border-2 rounded-lg active:bg-violet-700"
            @click="emit('start')"
            >
            Start
        </button>
        <button
            v-if="canPause()"
            class="mt-4 py-3 px-6 text-xl border-2 rounded-lg active:bg-violet-700 "
            @click="emit('pause')"
            >
            Pause
        </button>
    </div>
</template>

<script setup lang="ts">
    const props = defineProps({
        status: Object,
    });
    const emit = defineEmits(['start', 'pause']);

    function canStart() {
        return ((props.status?.st == 1) ||
                (props.status?.st == 3) ||
                (props.status?.st == 4));
    }

    function canPause() {
        return (props.status?.st == 2);
    }

    function mode() {
        switch(props.status?.md) {
            case 1: return "Clock";
            case 2: return "EMOM";
            case 3: return "For Time";
            case 4: return "AMRAP";
        }
        return "?";
    }

    function status() {
        switch(props.status?.st) {
            case 1: return "Stand By";
            case 2: return "Run";
            case 3: return "Pause";
            case 4: return "Complete";
        }
        return "?";
    }

    function time() {
        return String(props.status?.h).padStart(2,'0') + ":" +
               String(props.status?.m).padStart(2,'0') + ":" +
               String(props.status?.s).padStart(2,'0');
    }

</script>
