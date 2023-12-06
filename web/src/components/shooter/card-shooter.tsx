// "use client";

import { Button } from "@nextui-org/react";
import { useNavigate } from "react-router-dom";

export default function CardShooter() {
  const navigate = useNavigate();

  return (
    <div>
      <div className="mx-5 mt-5 mb-5 flex gap-x-5 justify-end items-center">
        <p className="grow mr-5 text-3xl font-serif font-bold">Interval Shooting</p>
        <Button
          radius="full"
          variant="flat"
          color="success"
          onPress={() => navigate("/shooter")}
        >
          Setup
        </Button>
      </div>

      <div className="mx-5 mb-5">
        <p>
          Save captured image to
          <span className="font-bold"> SD Card </span>
          regularly
        </p>
      </div>
    </div>
  );
}
