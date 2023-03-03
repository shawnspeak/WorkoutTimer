<template>
    <div class="flex flex-col justify-center dark:border-white">
        <div v-if="mode == 'up'">AMRAP</div>
        <div v-if="mode == 'down'">For Time</div>

        <select v-model="time" class="dark:bg-black">
            <option
                    v-for="(option, i) in getOptions()"
                    :value="option.secs"
                    :key="i"
                    >{{ option.text }}</option>
        </select>

        <button
            class="mt-4 py-3 px-6 text-xl border-2 rounded-lg active:bg-violet-700"
            @click="sendCommand">
            Select
        </button>

        <button
            class="mt-4 py-3 px-6 text-xl border-0 rounded-lg active:bg-violet-700"
            @click="emit('cancel')"
            >
            Cancel
        </button>
    </div>
</template>

<script setup lang="ts">
    import { ref } from 'vue'

    const emit = defineEmits(['commit', 'cancel'])
    const props = defineProps({
        mode: String,
    });

    const time = ref(600);

    function getOptions() {
        let ret = [];
        for(let i=30;i<3600;i+=30) {
            ret.push({
                secs: i,
                text: String(Math.floor(i/60)).padStart(2,'0') + ':' + String(i%60).padStart(2, '0')
             });
        }
        return ret;
    }

    function sendCommand() {
        if (props.mode == 'up') {
            emit('commit', 'up;' + time.value + ';');
        } else if (props.mode == 'down') {
            emit('commit', 'down;' + time.value + ';');
        }
    }
</script>
