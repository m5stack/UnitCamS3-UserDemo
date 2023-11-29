// "use client";

import { Switch } from "@nextui-org/react";
import { useState } from "react";

type LedState = "ON" | "OFF";

export default function CardLedTest() {
  const [ledState, setLedState] = useState("ON" as LedState);

  function handleLedStateSwitch(params: boolean) {
    console.log("change led state: " + params);

    if (params) {
      fetch("/api/v1/led_on");
    } else {
      fetch("/api/v1/led_off");
    }

    setLedState(params ? "ON" : "OFF");
  }

  return (
    <div>
      <div className="mx-5 mt-5 mb-5 flex gap-x-5 justify-end items-center">
        <p className="grow mr-5 text-3xl font-serif font-bold">LED</p>
        <Switch
          isSelected={ledState === "ON"}
          color="success"
          onValueChange={handleLedStateSwitch}
        ></Switch>
      </div>

      <div className="mx-5 mb-5">
        <p>{"LED is " + ledState}</p>
      </div>
    </div>
  );
}
