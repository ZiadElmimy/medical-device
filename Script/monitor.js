import {readData} from "./firebase.js";

const refreshElement = document.querySelector('.refresh-icon');
const heartRateElement = document.querySelector('.heart-rate-value');
const spo2Element = document.querySelector('.spo2-value');
const temperatureElement = document.querySelector('.temperature-value');
const upIndicatorElements = document.querySelectorAll('.up-icon');
const downIndicatorElements = document.querySelectorAll('.down-icon');


refreshElement.addEventListener('click', () => {
   readData().then((data) => {
       heartRateElement.innerHTML = data.heart_rate;
       spo2Element.innerHTML = data.spo2;
       temperatureElement.innerHTML = data.temperature;

       upIndicatorElements.forEach(indicator => {
           if(indicator.dataset.sensorMeasure === 'heart-rate') {
               if(data.heart_rate >= 60 && data.heart_rate <= 100) {
                   indicator.classList.remove('up-icon-disabled');
                   console.log('normal heart rate');
               } else {
                   indicator.classList.add('up-icon-disabled');
                   console.log('bad heart rate');
               }
           }

           if(indicator.dataset.sensorMeasure === 'spo2') {
               if(data.spo2 >= 95 && data.spo2 <= 100) {
                   indicator.classList.remove('up-icon-disabled');
                   console.log('normal SPO2');
               } else {
                   indicator.classList.add('up-icon-disabled');
                   console.log('bad SPO2');
               }
           }

           if(indicator.dataset.sensorMeasure === 'temperature') {
               if(data.temperature >= 36.1 && data.temperature <= 37.2) {
                   indicator.classList.remove('up-icon-disabled');
                   console.log('normal temperature');
               } else {
                   indicator.classList.add('up-icon-disabled');
                   console.log('bad temperature');
               }
           }
       });

       downIndicatorElements.forEach(indicator => {
           if(indicator.dataset.sensorMeasure === 'heart-rate') {
               if(data.heart_rate >= 60 && data.heart_rate <= 100) {
                   indicator.classList.add('down-icon-disabled');
                   console.log('normal heart rate');
               } else {
                   indicator.classList.remove('down-icon-disabled');
                   console.log('bad heart rate');
               }
           }

           if(indicator.dataset.sensorMeasure === 'spo2') {
               if(data.spo2 >= 95 && data.spo2 <= 100) {
                   indicator.classList.add('down-icon-disabled');
                   console.log('normal SPO2');
               } else {
                   indicator.classList.remove('down-icon-disabled');
                   console.log('bad SPO2');
               }
           }

           if(indicator.dataset.sensorMeasure === 'temperature') {
               if(data.temperature >= 36.1 && data.temperature <= 37.2) {
                   indicator.classList.add('down-icon-disabled');
                   console.log('normal temperature');
               } else {
                   indicator.classList.remove('down-icon-disabled');
                   console.log('bad temperature');
               }
           }
       });
   });

})