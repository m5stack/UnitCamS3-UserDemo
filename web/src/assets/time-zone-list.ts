interface TimeZoneObject {
  label: string;
  value: string;
}

export const timeZoneList: TimeZoneObject[] = [
  // UTC-...
  { label: "(GMT-12) Pacific/Wake", value: "UTC-12" },
  { label: "(GMT-11) Pacific/Midway", value: "UTC-11" },
  { label: "(GMT-10) Pacific/Honolulu", value: "UTC-10" },
  { label: "(GMT-9) America/Adak", value: "UTC-9" },
  { label: "(GMT-8) America/Anchorage", value: "UTC-8" },
  { label: "(GMT-7) America/Los_Angeles", value: "UTC-7" },
  { label: "(GMT-6) America/Denver", value: "UTC-6" },
  { label: "(GMT-5) America/Chicago", value: "UTC-5" },
  { label: "(GMT-4) America/New_York", value: "UTC-4" },
  { label: "(GMT-3) America/Buenos_Aires", value: "UTC-3" },
  { label: "(GMT-2) America/Noronha", value: "UTC-2" },
  { label: "(GMT-1) Atlantic/Azores", value: "UTC-1" },

  // UTC+...
  { label: "(GMT+0) Europe/London", value: "UTC+0" },
  { label: "(GMT+1) Europe/Berlin", value: "UTC+1" },
  { label: "(GMT+2) Europe/Athens", value: "UTC+2" },
  { label: "(GMT+3) Europe/Moscow", value: "UTC+3" },
  { label: "(GMT+4) Asia/Dubai", value: "UTC+4" },
  { label: "(GMT+5) Asia/Karachi", value: "UTC+5" },
  { label: "(GMT+5:30) Asia/Kolkata", value: "UTC+5:30" },
  { label: "(GMT+6) Asia/Dhaka", value: "UTC+6" },
  { label: "(GMT+7) Asia/Bangkok", value: "UTC+7" },
  { label: "(GMT+8) Asia/Shanghai", value: "UTC+8" },
  { label: "(GMT+9) Asia/Tokyo", value: "UTC+9" },
  { label: "(GMT+9:30) Australia/Adelaide", value: "UTC+9:30" },
  { label: "(GMT+10) Australia/Sydney", value: "UTC+10" },
  { label: "(GMT+11) Pacific/Noumea", value: "UTC+11" },
  { label: "(GMT+12) Pacific/Auckland", value: "UTC+12" },
];
