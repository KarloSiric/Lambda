# CRITICAL BUG FOUND: Linear Euler Interpolation

The current code does:
1. Calculate rotation as: `base + delta` where delta is interpolated
2. Convert to quaternion
3. Build matrix

But Valve does:
1. Calculate angle1 and angle2 (raw values for two frames)
2. Scale: `angle1 = base + raw1 * scale; angle2 = base + raw2 * scale`
3. Convert BOTH to quaternions: `AngleQuaternion(angle1, q1); AngleQuaternion(angle2, q2);`
4. SLERP: `QuaternionSlerp(q1, q2, s, q);`
5. Build matrix from final quaternion

**YOU CANNOT LINEARLY INTERPOLATE EULER ANGLES!** This causes gimbal lock and distortion!

## Fix Required

Modify `calc_bone_anim_value()` to return TWO values (value1, value2) instead of one interpolated value.
Then in the main loop:
- Get raw value1 and value2 for each rotation channel
- Build angle1[3] and angle2[3]
- Convert to q1 and q2
- SLERP to get final q
- Convert q to matrix

This matches Valve's implementation exactly.
