<template>
    <div class="knob">
      <div class="dial" :style="{ transform: 'rotate(' + angle + 'deg)' }">
        <div class="marker"></div>
      </div>
      <div class="label">{{ value }}</div>
    </div>
  </template>
  
  <script>
  export default {
    name: 'DialKnob',
    props: {
      value: {
        type: Number,
        required: true,
      },
      min: {
        type: Number,
        default: 0,
      },
      max: {
        type: Number,
        default: 100,
      },
    },
    computed: {
      angle() {
        const value = Math.max(this.min, Math.min(this.max, this.value));
        return ((value - this.min) / (this.max - this.min)) * 270 - 135;
      },
    },
  };
  </script>
  
  <style>
  .knob {
    display: flex;
    flex-direction: column;
    align-items: center;
    font-family: Arial, sans-serif;
    color: #333;
  }
  
  .dial {
    position: relative;
    width: 80px;
    height: 80px;
    border-radius: 50%;
    border: 1px solid #333;
    padding: 10px;
    box-sizing: border-box;
    transform-origin: center center;
  }
  
  .marker {
    position: absolute;
    top: 50%;
    left: 50%;
    width: 4px;
    height: 20px;
    background-color: #333;
    transform: translate(-50%, -50%);
  }
  
  .label {
    font-size: 14px;
    margin-top: 10px;
  }
  </style>
  